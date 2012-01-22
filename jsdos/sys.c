#include "kernel.h"
#include "myjit/jitlib.h"

// pointer to a function accepting one argument of type long and returning long value
typedef int64_t (* plfl)(int64_t);

// Input a byte from a port
inline unsigned char inportb(unsigned int port)
{
   unsigned char ret;
   __asm__ volatile ("inb %%dx,%%al":"=a" (ret):"d" (port));
   return ret;
}

// Output a byte to a port
inline void outportb(unsigned int port, unsigned char value)
{
   __asm__ volatile ("outb %%al,%%dx": :"d" (port), "a" (value));
}

extern void hwi_exception_default();
extern void hwi_interrupt_default();
extern void hwi_exception_gate_00();
extern void hwi_keyboard();
extern void hwi_rtc();
extern void hwi_network();
extern void hwi_ap_wakeup();
extern void hwi_ap_reset();

void idt_set(uintptr_t i, uintptr_t p)
{
//     char tmp[17];
//     memcpy(tmp, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 17);
//     kprint(txt_format_hex_int64(tmp, i));
//     kprint(" = ");
//     kprint(txt_format_hex_int64(tmp, p));
//     kprint("\n");

    char *idt = (char *)(i << 4);

    *(uint16_t *)idt = p;
    idt += 2;

    p >>= 16;
    idt += 4;

    *(uint16_t *)idt = p;
    idt += 2;

    p >>= 16;
    *(uint32_t *)idt = p;
}

void ioapic_reg_write(uint32_t index, uint32_t data)
{
    *(uint32_t *)(*os_IOAPIC + 0x00) = index;
    *(uint32_t *)(*os_IOAPIC + 0x10) = data;
}

void ioapic_entry_write(uint32_t index, uint64_t data)
{
    ioapic_reg_write((index << 1) + 0x10, (uint32_t) (data));
    ioapic_reg_write((index << 1) + 0x11, (uint32_t) (data >> 32));
}

void smp_reset(uint8_t apicId)
{
    // Set APIC id:
    *(uint32_t *)(*os_localAPIC + 0x310) = (uint32_t)apicId << 24;
    // Execute interrupt 0x81:
    *(uint32_t *)(*os_localAPIC + 0x300) = (uint32_t)0x81;
}

void init_memory_map()
{
    // TODO(jsd): set up memory page tables

    // Stack starts at the top and grows downwards in memory addresses:
    *os_StackBase = (char *)0x200000 + 0x200000;
    mem_next = (void *)((char *)0x200000 + 0x200000 * (*os_NumCores));
}

// Called from start() to initialize 64-bit hardware:
void init_64()
{
    char tmp[17];
    memcpy(tmp, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 17);

    // Setup catch-all exception and interrupt handlers:
    for (uintptr_t i = 0; i < 32; ++i)
    {
        idt_set(i, (uintptr_t)hwi_exception_default);
    }
    for (uintptr_t i = 32; i < 256; ++i)
    {
        idt_set(i, (uintptr_t)hwi_interrupt_default);
    }

    // Setup specific exception handlers:
    for (uintptr_t i = 0; i < 20; ++i)
    {
        idt_set(i, ((uintptr_t)hwi_exception_gate_00) + (i << 4));
    }

    idt_set(0x21, (uintptr_t)hwi_keyboard);
    idt_set(0x28, (uintptr_t)hwi_rtc);
    idt_set(0x80, (uintptr_t)hwi_ap_wakeup);
    idt_set(0x81, (uintptr_t)hwi_ap_reset);

    kprint("Exception and interrupt handlers set up.\n");

    // Grab data from Pure64:
    *os_localAPIC = *(char **)0x5000;
    *os_IOAPIC = *(char **)0x5008;
    *os_NumCores = *(uint16_t *)0x5012;

    kprint("local-APIC = 0x");
    kprint(txt_format_hex_int64(tmp, (int64_t)*os_localAPIC));
    kprint("\nIO-APIC   = 0x");
    kprint(txt_format_hex_int64(tmp, (int64_t)*os_IOAPIC));
    kprint("\n");

    init_memory_map();

    kprint("Resetting APs...\n");

    // Reset all active cores except BSP:
    for (int i = 0; i < 8; ++i)
    {
        // Get the CPU flags:
        uint8_t flags = ((uint8_t *)0x5700)[i];

        kprint(txt_format_hex_int8(tmp + 14, i));
        kprint(": flags = ");
        kprint(txt_format_hex_int8(tmp + 14, flags));
        kprint("  ");

        // Core not active?
        if ((flags & 1) == 0)
        {
            kprint("not active\n");
            continue;
        }

        // Skip BSP:
        if ((flags & 2) == 2)
        {
            kprint("skip bsp\n");
            continue;
        }

        // Reset this AP:
        kprint("reset AP\n");
        smp_reset(i);
    }

    kprint("APs reset\n");

    // Enable cascade, keyboard interrupts:
    inportb(0x21);
    outportb(0x21, 0b11111001);
    // Enable RTC:
    inportb(0xA1);
    outportb(0xA1, 0b11111110);

    kprint("Interrupts enabled\n");

    // Reset keyboard and empty the buffer
    outportb(0x64, 0x20);
    uint8_t ct = (inportb(0x60) | 0b00000001) & 0b11101111;
    outportb(0x64, 0x60);
    outportb(0x60, ct);
}

// Called first from start to init the system.
int sys_init()
{
    // Clear the text screen:
    hw_txt_clear_screen();

    // Initialize 64-bit hardware:
    init_64();

    // Quick VGA register access to disable the hardware text cursor:
    outportb(0x03D4, 0x0A);
    int x = inportb(0x03D5);
    outportb(0x03D5, x | (1 << 5));

    // Return 0 to indicate successful initialization.
    return 0;
}

// Called to halt the system indefinitely.
void sys_halt()
{
    // TODO(jsd): Want an actual CPU sleep here to save power.
    while (1) { }
}

// Called from main to run the system after `sys_init`.
int sys_run()
{
    kprint("\n\n");
//     kprint("123456789012345678901234567890123456789012345678901234567890123456789012345678901\n");
//     kprint("hello\n\na\nb\ncdef");
//     kprint("\n\n");

    // creates a new instance of the compiler
    struct jit * p = jit_init();

    plfl foo;

    // the code generated by the compiler will be assigned to the function `foo'
    jit_prolog(p, &foo);

    // the first argument of the function
    jit_declare_arg(p, JIT_SIGNED_NUM, sizeof(int64_t));

    // moves the first argument into the register R(0)
    jit_getarg(p, R(0), 0);

    // takes the value in R(0), increments it by one, and stores the result into the
    // register R(1)
    jit_addi(p, R( 1), R( 0), 1);
    jit_addi(p, R( 2), R( 1), 1);
    // NOTE(jsd): testing the "unlimited" virtual register system here:
    jit_addi(p, R( 3), R( 2), 1);
    jit_addi(p, R( 4), R( 3), 1);
    jit_addi(p, R( 5), R( 4), 1);
    jit_addi(p, R( 6), R( 5), 1);
    jit_addi(p, R( 7), R( 6), 1);
    jit_addi(p, R( 8), R( 7), 1);
    jit_addi(p, R( 9), R( 8), 1);
    jit_addi(p, R(10), R( 9), 1);
    jit_addi(p, R(11), R(10), 1);
    jit_addi(p, R(12), R(11), 1);
    jit_addi(p, R(13), R(12), 1);
    jit_addi(p, R(14), R(13), 1);
    jit_addi(p, R(15), R(14), 1);
    jit_addi(p, R(16), R(15), 1);
    jit_addi(p, R(17), R(16), 1);
    jit_addi(p, R(18), R(17), 1);
    // NOTE(jsd): 20 should be enough to pass any hardware register count:
    jit_addi(p, R(19), R(18), 1);

    // returns from the function and returns the value stored in the register R(19)
    jit_retr(p, R(19));

    // compiles the above defined code
    jit_generate_code(p);

    char intfmt[17];
    memcpy(intfmt, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 17);

    size_t pos;
    hw_txt_set_color(0x07); kprint("foo(  1): 0x");
    hw_txt_set_color(0x0F); kprint(txt_format_hex_int64(intfmt, foo(1)));
    kprint("\n");
    hw_txt_set_color(0x07); kprint("foo(100): 0x");
    hw_txt_set_color(0x0F); kprint(txt_format_hex_int64(intfmt, foo(100)));
    kprint("\n");
    hw_txt_set_color(0x07); kprint("foo(255): 0x");
    hw_txt_set_color(0x0F); kprint(txt_format_hex_int64(intfmt, foo(255)));
    kprint("\n\n");

    // if you are interested, you can dump the machine code
    // this functionality is provided through the `gcc' and `objdump'
    // jit_dump_code(p, 0);

    // Dump memory malloc'd:
    hw_txt_set_color(0x09);
    kprint("malloc'd: 0x");
    kprint(txt_format_hex_int64(intfmt, mem_get_alloced()));
    kprint("\n");

    // cleanup
    jit_free(p);

    hw_txt_set_color(0x09);
    kprint("malloc'd: 0x");
    kprint(txt_format_hex_int64(intfmt, mem_get_alloced()));
    kprint("\n");

    hw_txt_set_color(0x07);

    mem_walk_leaked();

    //memcpy(intfmt, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 17);
    kprint("leaked: 0x");
    kprint(txt_format_hex_int64(intfmt, mem_get_alloced()));
    kprint("\n");

    kprint("done");

    return 0;
}

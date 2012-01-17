#include "kernel.h"
#include "myjit/jitlib.h"

// pointer to a function accepting one argument of type long and returning long value
typedef int64_t (* plfl)(int64_t);

// Called first from main to init the system.
int sys_init()
{
    // Clear the text screen:
    hw_txt_clear_screen();

    // Return 0 to indicate successful initialization.
    return 0;
}

void sys_done()
{
    // NOTE(jsd): for testing purposes
    //assert(0);
}

// Called to halt the system indefinitely.
void sys_sleep()
{
    // TODO(jsd): Want an actual CPU sleep here to save power.
    while (1) { }
}

void visit_allocation_block(void *pblock)
{
    char intfmt[17] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
    mem_alloc_t *b = (mem_alloc_t *)pblock;

    hw_txt_vscroll_up(1);
    uint lastrow = hw_txt_get_rows() - 1;

    size_t pos = hw_txt_write_string("0x", lastrow, 0, 0x7);
    pos += hw_txt_write_string(txt_format_hex_int64(intfmt, (int64_t)b), lastrow, pos, 0x7);
#ifdef JSDOS_DEBUG
    // Dump the malloc() call location that allocated the leaked block:
    pos += hw_txt_write_string(": ", lastrow, pos, 0x7);
    pos += hw_txt_write_string(b->loc_malloc.file, lastrow, pos, 0x7);
    pos += hw_txt_write_string(" (", lastrow, pos, 0x7);
    pos += hw_txt_write_string(txt_format_hex_int64(intfmt, b->loc_malloc.line) + 4, lastrow, pos, 0x7);
    pos += hw_txt_write_string(") ", lastrow, pos, 0x7);
    pos += hw_txt_write_string(b->loc_malloc.function, lastrow, pos, 0x7);
#endif
}

// Called from main to run the system after `sys_init`.
int sys_run()
{
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
    // NOTE(jsd): testing the "unlimited" virtual register system here:
    jit_addi(p, R( 2), R( 1), 1);
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

    uint lastrow = hw_txt_get_rows() - 1;

    char intfmt[17] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
    size_t pos;
    pos = hw_txt_write_string("foo(  1): ", 2, 0, 0x7);
    hw_txt_write_string(txt_format_hex_int64(intfmt, foo(1)), 2, 0 + pos, 0xf);
    pos = hw_txt_write_string("foo(100): ", 3, 0, 0x7);
    hw_txt_write_string(txt_format_hex_int64(intfmt, foo(100)), 3, 0 + pos, 0xf);
    pos = hw_txt_write_string("foo(255): ", 4, 0, 0x7);
    hw_txt_write_string(txt_format_hex_int64(intfmt, foo(255)), 4, 0 + pos, 0xf);

    // if you are interested, you can dump the machine code
    // this functionality is provided through the `gcc' and `objdump'
    // jit_dump_code(p, 0);

    // Dump memory malloc'd:
    pos = hw_txt_write_string("malloc'd: 0x", lastrow - 2, 0, 0x9);
    hw_txt_write_string(txt_format_hex_int64(intfmt, mem_get_alloced()), lastrow - 2, pos, 0x9);

    // cleanup
    jit_free(p);

    pos = hw_txt_write_string("malloc'd: 0x", lastrow - 1, 0, 0x9);
    hw_txt_write_string(txt_format_hex_int64(intfmt, mem_get_alloced()), lastrow - 1, pos, 0x9);

    mem_walk_leaked(&visit_allocation_block);

#if 0
    hw_txt_clear_row(lastrow);
    hw_txt_write_string("done", lastrow, 0, 0xf);

    // Test the vertical scroll function:
    hw_txt_vscroll_up(2);
#endif
    return 0;
}

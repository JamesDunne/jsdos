#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

// Pure64 constants:
////////////////////

// For Pure64, system code is loaded at 0x100000:
#define MEM_CODE (0x100000ULL)
// Reserve 32MiB for system code:
#define MEM_DATA (MEM_CODE + (0x100000ULL * 31ULL))

char **os_localAPIC;
char **os_IOAPIC;
char **os_StackBase;
uint16_t *os_NumCores;

// Functions:
/////////////

int kprint(const char *s);

// Simple function pointers:
typedef void (* action1_v_fp)(void *p1);

// Hardware interrupts:
///////////////////////

void hw_int_rtc();
void hw_int_keyboard();
void hw_int_network();
void hw_ap_clear();

// Debugging:
/////////////

#if JSDOS_DEBUG
typedef struct {
    bool        avail;
    const char  *file;
    uint        line;
    const char  *function;
} debugloc_t;
#endif

// Memory:
//////////

typedef struct {
    size_t      allocated;
    size_t      freed;
#if JSDOS_DEBUG
    // Track source code locations where malloc() and free() were called:
    debugloc_t  loc_malloc;
    debugloc_t  loc_free;
#endif
    // chunk of malloc'd memory immediately follows.
} mem_alloc_t;

void *mem_next;

size_t mem_get_alloced();
void mem_walk_leaked();

// Hardware 80x25 text-mode screen functions:
/////////////////////////////////////////////

void hw_txt_clear_row(uint row);
void hw_txt_clear_screen();
uint hw_txt_get_rows();
uint hw_txt_get_cols();
void hw_txt_vscroll_up(uint rows);
void hw_txt_set_color(uint8_t color);

// Formatting functions:
////////////////////////
const char tbl_hex[16];

char *txt_format_hex_int8(char dst[2], int8_t n);
char *txt_format_hex_int16(char dst[4], int16_t n);
char *txt_format_hex_int32(char dst[8], int32_t n);
char *txt_format_hex_int64(char dst[16], int64_t n);

char *txt_format_uint64(char dst[21], int base, uint64_t n);

#include "sys.h"

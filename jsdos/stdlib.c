#include "kernel.h"

// stdlib "implementations":
/////////////////////////////

// Ladies and gentlemen, the dumbest malloc ever:

void *mem_next = (void *)MEM_DATA;
size_t mem_alloced = 0;

void *malloc_dbg(size_t size, const char *file, uint line, const char *function)
{
    // Book-keeping:
    mem_alloc_t *r = (mem_alloc_t *)mem_next;
    r->allocated = size;
    r->freed = 0;
#ifdef JSDOS_DEBUG
    r->loc_malloc.avail = true;
    r->loc_malloc.file = file;
    r->loc_malloc.line = line;
    r->loc_malloc.function = function;
    r->loc_free.avail = false;
#endif
    mem_alloced += size;

    void *ptr = (void *)((char *)r + sizeof(mem_alloc_t));
    mem_next = (void *)((char *)ptr + size);

    // Clear out the tail malloc record:
    mem_alloc_t *tail = (mem_alloc_t *)mem_next;
    tail->allocated = 0;
    tail->freed = 0;

    return ptr;
}

#ifndef JSDOS_DEBUG

void *malloc(size_t size)
{
    return malloc_dbg(size, __FILE__, __LINE__, __FUNCTION__);
}

#endif

void *realloc(void *ptr, size_t size)
{
    free(ptr);
    return malloc(size);
}

void free_dbg(void *ptr, const char *file, uint line, const char *function)
{
    mem_alloc_t *r = (mem_alloc_t *)((char *)ptr - sizeof(mem_alloc_t));
    if (r->freed > 0) assert("double free!");

    // Book-keeping:
    mem_alloced -= r->allocated;
    r->freed = r->allocated;
#ifdef JSDOS_DEBUG
    r->loc_free.avail = true;
    r->loc_free.file = file;
    r->loc_free.line = line;
    r->loc_free.function = function;
#endif
}

#ifndef JSDOS_DEBUG

void free(void *ptr)
{
    free_dbg(ptr, __FILE__, __LINE__, __FUNCTION__);
}

#endif

size_t mem_get_alloced() { return mem_alloced; }

void mem_walk_leaked(action1_v_fp visit)
{
    // Start at the first block:
    mem_alloc_t *c = (mem_alloc_t *)MEM_DATA;

    while (c->allocated != 0)
    {
        // Block is still allocated:
        if (c->freed == 0)
        {
            visit((void *)c);
        }

        // Move to next block:
        c = (mem_alloc_t *)((char *)c + sizeof(mem_alloc_t) + c->allocated);
    }
}

void *memcpy(void *dst, const void *src, size_t n)
{
    // TODO(jsd): a combo approach of copying machine words followed by remainder bytes
#if 0
    intptr_t *dstp = dst;
    const intptr_t *srcp = src;
    for (size_t i = 0; i < n; i += sizeof(intptr_t))
        *dstp++ = *srcp++;
#else
    char *dstp = dst;
    const char *srcp = src;
    for (size_t i = 0; i < n; ++i)
        *dstp++ = *srcp++;
#endif
    return dst;
}

// Called by assert(n) macro, expected to halt execution:
void __assert_fail(const char *assertion, const char *file, unsigned int line, const char *function)
{
    uint lastrow = hw_txt_get_rows() - 1;

    hw_txt_clear_row(lastrow - 1);
    hw_txt_clear_row(lastrow);

    char tmp[9] = "\0\0\0\0\0\0\0\0\0";
    size_t pos = 0;

    if (file == NULL) file = "<no file>";
    pos += hw_txt_write_string("assertion failure in ", lastrow - 1, pos, 0xf);
    pos += hw_txt_write_string(file, lastrow - 1, pos, 0xf);
    pos += hw_txt_write_string(" (", lastrow - 1, pos, 0xf);
    pos += hw_txt_write_string(txt_format_hex_int32(tmp, line), lastrow - 1, pos, 0xf);
    pos += hw_txt_write_string("): ", lastrow - 1, pos, 0xf);
    if (function == NULL) function = "<no function>";
    pos += hw_txt_write_string(function, lastrow - 1, pos, 0xf);

    if (assertion == NULL) assertion = "<no assertion>";
    hw_txt_write_string(assertion, lastrow, 0, 0xf);

    // Halt the system:
    sys_sleep();
}

int printf(const char * format, ...)
{
    // TODO(jsd): handle va_args
    hw_txt_write_string(format, 23, 0, 0xf);
    return 0;
}

#if __USE_FORTIFY_LEVEL > 1
// Apparently only used when -O flag passed to gcc.
int __printf_chk (int __flag, __const char *__restrict __format, ...)
{
    return 0;
}
#endif

#if JITLIB_DEBUG

// NOTE(jsd): This was here for jitlib-debug.c inclusion.
char *strcat(char *dst, const char *src)
{
    char *dst_orig = dst;
    while (*dst != 0) dst++;
    while (*src != 0) *dst++ = *src++;
    *dst = *src;
    return dst_orig;
}

char *strcpy(char *dst, const char *src)
{
    char *dst_orig = dst;
    while (*src != 0) *dst++ = *src++;
    *dst = *src;
    return dst_orig;
}

size_t strlen(const char *s)
{
    size_t l = 0;
    while (*s++ != 0) ++l;
    return l;
}

#endif

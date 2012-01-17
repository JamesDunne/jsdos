#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include "kernel.h"

// stdlib "implementations":
/////////////////////////////

// Ladies and gentlemen, the dumbest malloc ever:

void *malloc(size_t size)
{
    static void *next = (void *)MEM_DATA;
    void *ptr = next;
    next = (void*)((char *)next + size);
    return ptr;
}

void *realloc(void *ptr, size_t size)
{
    return malloc(size);
}

void free(void *ptr)
{
    // Nothing to do
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

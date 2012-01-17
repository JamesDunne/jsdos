#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include "kernel.h"

// stdlib "implementations":
/////////////////////////////

// Ladies and gentlemen, the dumbest malloc ever:

void *malloc(size_t size)
{
    static void *next = MEM_DATA;
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
    // Nothing to do since
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
    hw_txt_write_string(assertion, 24, 0, 0xf);

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

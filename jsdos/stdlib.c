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
    mem_alloced += r->allocated;

    void *ptr = (void *)((char *)r + sizeof(mem_alloc_t));
    mem_next = (void *)((char *)ptr + size);

    // Clear out the tail malloc record:
    mem_alloc_t *tail = (mem_alloc_t *)mem_next;
    tail->allocated = 0;
    tail->freed = 0;

    return ptr;
}

void *realloc(void *ptr, size_t size)
{
    free(ptr);
    return malloc(size);
}

void free_dbg(void *ptr, const char *file, uint line, const char *function)
{
    mem_alloc_t *r = (mem_alloc_t *)((char *)ptr - sizeof(mem_alloc_t));
    assert(r->allocated > 0);
    assert(r->freed == 0);

    // Book-keeping:
    mem_alloced -= r->allocated;
    r->freed = r->allocated;
#ifdef JSDOS_DEBUG
    r->loc_free.avail = true;
    r->loc_free.file = file;
    r->loc_free.line = line;
    r->loc_free.function = function;
#endif

    memset(ptr, 0xFC, r->allocated);
}

size_t mem_get_alloced() { return mem_alloced; }

void mem_walk_leaked()
{
    char tmp[17];
    memcpy(tmp, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 17);

    // Start at the first block:
    mem_alloc_t *c = (mem_alloc_t *)MEM_DATA;

    while (c->allocated != 0)
    {
        // Block is still allocated:
        if (c->freed == 0)
        {
            printf("0x");
            printf(txt_format_hex_int64(tmp, (int64_t)((char *)c + sizeof(mem_alloc_t))) + 8);
            printf(", 0x");
            printf(txt_format_hex_int64(tmp, c->allocated) + 12);
#ifdef JSDOS_DEBUG
            printf(" in ");
            printf(c->loc_malloc.function);
            printf(" (");
            printf(c->loc_malloc.file);
            printf(":0x");
            printf(txt_format_hex_int64(tmp, c->loc_malloc.line) + 12);
            printf(")");
#endif
            printf("\n");
        }
        else
        {
#if 0
            printf("F 0x");
            printf(txt_format_hex_int64(tmp, (int64_t)((char *)c + sizeof(mem_alloc_t))) + 8);
            printf(", 0x");
            printf(txt_format_hex_int64(tmp, c->allocated) + 12);
            printf("\n");
#endif
        }

        // Move to next block:
        c = (mem_alloc_t *)((char *)c + sizeof(mem_alloc_t) + c->allocated);
    }
}

#ifndef JSDOS_DEBUG

void *malloc(size_t size)
{
    return malloc_dbg(size, __FILE__, __LINE__, __FUNCTION__);
}

void free(void *ptr)
{
    free_dbg(ptr, __FILE__, __LINE__, __FUNCTION__);
}

#endif

void *memcpy(void *dst, const void *src, size_t n)
{
    // TODO(jsd): a combo approach of copying machine words followed by remainder bytes
#if 0
    intptr_t *dstp = dst;
    const intptr_t *srcp = src;
    for (size_t i = 0; i < n; i += sizeof(intptr_t))
        *dstp++ = *srcp++;
#else
    char *dstp = (char *)dst;
    const char *srcp = (const char *)src;
    for (size_t i = 0; i < n; ++i)
        *dstp++ = *srcp++;
#endif
    return dst;
}

void *memset(void *s, int c, size_t n)
{
    char *dstp = (char *)s;
    for (size_t i = 0; i < n; ++i)
        *dstp++ = c;
    return s;
}

// Called by assert(n) macro, expected to halt execution:
void __assert_fail(const char *assertion, const char *file, unsigned int line, const char *function)
{
    char tmp[9];
    memcpy(tmp, "\0\0\0\0\0\0\0\0\0", 9);

    size_t pos = 0;

    if (file == NULL) file = "<no file>";
    if (function == NULL) function = "<no function>";
    if (assertion == NULL) assertion = "<no assertion>";

    hw_txt_set_color(0x0F);
    printf("\nassertion failure in ");
    printf(file);
    printf(" (");
    printf(txt_format_hex_int32(tmp, line));
    printf("): ");
    printf(function);
    printf("\n");

    printf(assertion);
    hw_txt_set_color(0x07);

    // Halt the system:
    sys_halt();
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

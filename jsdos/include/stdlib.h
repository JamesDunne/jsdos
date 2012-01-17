
#ifndef _JSDOS_STDLIB_H

// Include system header file:
#include <jsdos_host/stdlib.h>

typedef unsigned int uint;

#ifdef JSDOS_DEBUG

extern void *malloc_dbg(size_t size, const char *file, uint line, const char *function);
extern void free_dbg(void *ptr, const char *file, uint line, const char *function);

#undef malloc
#undef free

// Wrap malloc and free in macros to record source code locations of call-sites:
#define malloc(s) malloc_dbg((s), __FILE__, __LINE__, __FUNCTION__)
#define free(p) free_dbg((p), __FILE__, __LINE__, __FUNCTION__)

#endif

#define _JSDOS_STDLIB_H
#endif
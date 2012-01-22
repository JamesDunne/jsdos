#include "kernel.h"

int printf(const char * format, ...)
{
    kprint(format);
}

#if __USE_FORTIFY_LEVEL > 1

// NOTE(jsd): This special function is defined only when -O<n> flag is passed to gcc.
int __printf_chk (int __flag, __const char *__restrict __format, ...)
{
    return printf(__format);
}

#endif

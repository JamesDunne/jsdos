#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include "kernel.h"

// start must absolutely be the first function defined and linked:

void start()
{
    int c;
    if (sys_init() != 0) goto halt;
    if (sys_run() != 0) goto halt;

halt:
    sys_halt();
}

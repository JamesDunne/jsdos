#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include "kernel.h"

// main must absolutely be the first function defined and linked:

int main()
{
    int c;
    if ((c = sys_init()) != 0) return c;
    if ((c = sys_run()) != 0) return c;
    sys_done();
    sys_sleep();
}

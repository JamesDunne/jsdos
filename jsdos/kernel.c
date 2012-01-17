#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include "kernel.h"

// Kernel functions:
/////////////////////////////

const char tbl_hex[16] = "0123456789ABCDEF";

const char *txt_format_hex_int8(char dst[2], int8_t n)
{
    unsigned char *x = (unsigned char *)&n;
    dst[0] = tbl_hex[x[0] >> 4];
    dst[1] = tbl_hex[x[0] & 15];
    return dst;
}

const char *txt_format_hex_int16(char dst[4], int16_t n)
{
    unsigned char *x = (unsigned char *)&n;
    dst[0] = tbl_hex[x[1] >> 4];
    dst[1] = tbl_hex[x[1] & 15];
    dst[2] = tbl_hex[x[0] >> 4];
    dst[3] = tbl_hex[x[0] & 15];
    return dst;
}

const char *txt_format_hex_int32(char dst[8], int32_t n)
{
    unsigned char *x = (unsigned char *)&n;
    dst[0] = tbl_hex[x[3] >> 4];
    dst[1] = tbl_hex[x[3] & 15];
    dst[2] = tbl_hex[x[2] >> 4];
    dst[3] = tbl_hex[x[2] & 15];
    dst[4] = tbl_hex[x[1] >> 4];
    dst[5] = tbl_hex[x[1] & 15];
    dst[6] = tbl_hex[x[0] >> 4];
    dst[7] = tbl_hex[x[0] & 15];
    return dst;
}

char *hw_colortext = (char *) 0xb8000;

unsigned int hw_txt_write_string(const char *msg, uint row, uint col, uint8_t color)
{
    char *vidmem = hw_colortext;
    unsigned int i = 0;

    i = (row*80*2)+(col*2);

    while (*msg != 0)
    {
        vidmem[i] = *msg;
        msg++;
        i++;
        vidmem[i] = color;
        i++;
    }

    return 1;
}

void hw_txt_clear_row(uint row)
{
    for (unsigned int i = (row * 80 * 2); i < (row * 80 * 2) + (80 * 2); i += 2)
        hw_colortext[i] = 0;
}

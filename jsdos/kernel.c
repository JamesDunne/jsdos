#include "kernel.h"

// Hardware screen functions:
/////////////////////////////

// Memory mapped I/O for the 80x25 color text-mode screen:
volatile char *hw_txt_colorbuf = (char *) 0xb8000;
uint hw_txt_cols = 80;
uint hw_txt_rows = 25;

uint hw_txt_stdout_row = 24, hw_txt_stdout_col = 0;
uint hw_txt_scrolled = 0;
uint8_t hw_txt_color = 0x07;

// Clears an entire screen row.
void hw_txt_clear_row(uint row)
{
    for (size_t i = (row * hw_txt_cols * 2); i < (row * hw_txt_cols * 2) + (hw_txt_cols * 2); i += 2)
    {
        hw_txt_colorbuf[i+0] = 0;
        hw_txt_colorbuf[i+1] = 7;
    }
}

// Clears the entire screen.
void hw_txt_clear_screen()
{
    for (size_t i = 0; i < (hw_txt_rows * hw_txt_cols * 2); i += 2)
    {
        hw_txt_colorbuf[i+0] = 0;
        hw_txt_colorbuf[i+1] = 7;
    }
}

uint hw_txt_get_rows() { return hw_txt_rows; }
uint hw_txt_get_cols() { return hw_txt_cols; }

// Vertically scroll the screen up by `rows` rows.
void hw_txt_vscroll_up(uint rows)
{
    if (rows > hw_txt_rows) rows = hw_txt_rows;

    int offs = (rows * hw_txt_cols * 2);

    // Shift all the screen rows up:
    for (size_t i = offs; i < (hw_txt_rows * hw_txt_cols * 2); i += 2)
    {
        hw_txt_colorbuf[i+0 - offs] = hw_txt_colorbuf[i+0];
        hw_txt_colorbuf[i+1 - offs] = hw_txt_colorbuf[i+1];
    }

    // Clear the new rows at the bottom:
    for (size_t row = hw_txt_rows - rows; row < hw_txt_rows; ++row)
        hw_txt_clear_row(row);
}

volatile int _unused = 0;

// Artificial delay:
void delay()
{
    int j = 0;
    for (int i = 0; i < 2000000000; ++i)
    {
        ++j;
    }
    _unused = j;
}

void hw_txt_set_color(uint8_t color)
{
    hw_txt_color = color;
}

int printf(const char * format, ...)
{
    // TODO(jsd): handle va_args
    const char *msg = format;

    const char *p = msg;

    while (*p != 0)
    {
        if (*p == '\n')
        {
            // Move to the next row at column 0:
            ++hw_txt_stdout_row;
            hw_txt_stdout_col = 0;
        }

        // Check cursor column bounds:
        if (hw_txt_stdout_col >= hw_txt_cols)
        {
            ++hw_txt_stdout_row;
            hw_txt_stdout_col = 0;
        }

        // Check cursor row bounds:
        if (hw_txt_stdout_row >= hw_txt_rows)
        {
            if (++hw_txt_scrolled >= (hw_txt_rows/4))
            {
                delay();
                hw_txt_scrolled = 0;
            }

            // Scroll up:
            hw_txt_stdout_row = hw_txt_rows - 1;
            hw_txt_vscroll_up(1);

            hw_txt_stdout_col = 0;
        }

        if (*p != '\n')
        {
            // Write the character and its color:
            hw_txt_colorbuf[(hw_txt_stdout_row * hw_txt_cols * 2) + (hw_txt_stdout_col * 2) + 0] = *p;
            hw_txt_colorbuf[(hw_txt_stdout_row * hw_txt_cols * 2) + (hw_txt_stdout_col * 2) + 1] = hw_txt_color;

            // Advance the cursor:
            ++hw_txt_stdout_col;
        }

        ++p;
    }

    return 0;
}

#if __USE_FORTIFY_LEVEL > 1

// NOTE(jsd): This special function is defined only when -O<n> flag is passed to gcc.
int __printf_chk (int __flag, __const char *__restrict __format, ...)
{
    return printf(__format);
}

#endif

// Kernel functions:
/////////////////////////////

const char tbl_hex[16] = "0123456789ABCDEF";

char *txt_format_hex_int8(char dst[2], int8_t n)
{
    unsigned char *x = (unsigned char *)&n;
    dst[0] = tbl_hex[x[0] >> 4];
    dst[1] = tbl_hex[x[0] & 15];
    return dst;
}

char *txt_format_hex_int16(char dst[4], int16_t n)
{
    unsigned char *x = (unsigned char *)&n;
    dst[0] = tbl_hex[x[1] >> 4];
    dst[1] = tbl_hex[x[1] & 15];
    dst[2] = tbl_hex[x[0] >> 4];
    dst[3] = tbl_hex[x[0] & 15];
    return dst;
}

char *txt_format_hex_int32(char dst[8], int32_t n)
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

char *txt_format_hex_int64(char dst[16], int64_t n)
{
    unsigned char *x = (unsigned char *)&n;
    dst[ 0] = tbl_hex[x[7] >> 4];
    dst[ 1] = tbl_hex[x[7] & 15];
    dst[ 2] = tbl_hex[x[6] >> 4];
    dst[ 3] = tbl_hex[x[6] & 15];
    dst[ 4] = tbl_hex[x[5] >> 4];
    dst[ 5] = tbl_hex[x[5] & 15];
    dst[ 6] = tbl_hex[x[4] >> 4];
    dst[ 7] = tbl_hex[x[4] & 15];
    dst[ 8] = tbl_hex[x[3] >> 4];
    dst[ 9] = tbl_hex[x[3] & 15];
    dst[10] = tbl_hex[x[2] >> 4];
    dst[11] = tbl_hex[x[2] & 15];
    dst[12] = tbl_hex[x[1] >> 4];
    dst[13] = tbl_hex[x[1] & 15];
    dst[14] = tbl_hex[x[0] >> 4];
    dst[15] = tbl_hex[x[0] & 15];
    return dst;
}

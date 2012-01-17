
typedef unsigned int uint;

// For Pure64, system code is loaded at 0x100000:
#define MEM_CODE (0x100000ULL)
// Reserve 32MiB for system code:
#define MEM_DATA (MEM_CODE + (0x100000ULL * 31ULL))

const char tbl_hex[16];

const char *txt_format_hex_int8(char dst[2], int8_t n);
const char *txt_format_hex_int16(char dst[4], int16_t n);
const char *txt_format_hex_int32(char dst[8], int32_t n);

unsigned int hw_txt_write_string(const char *msg, uint row, uint col, uint8_t color);
void hw_txt_clear_row(uint row);

int sys_init();
int sys_run();
void sys_sleep();

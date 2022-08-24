#ifndef VGA_H
#define VGA_H
#include <kernel/types.h>
#define VGA_COLOR_BLACK 0
#define VGA_COLOR_BLUE  1
#define VGA_COLOR_GREEN 2
#define VGA_COLOR_CYAN  3
#define VGA_COLOR_RED   4
#define VGA_COLOR_MAGENTA 5
#define VGA_COLOR_BROWN 6
#define VGA_COLOR_LIGHTGRAY 7

void vga_install(uint32_t _buffer_addr,uint16_t _columns, uint16_t _rows,uint8_t _bpp);
void print_char(char c);
#endif
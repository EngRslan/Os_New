#include <kernel/types.h>
#include <kernel/bits.h>
#include <kernel/system.h>
#include <ctype.h>
#include <kernel/drivers/vga.h>
#define PIXEL(x,y)buffer_address[y*columns+x]
#define TEXTSTYLE(fg,fgb,bg,bgb)(((bgb & 0x1)<<7) | ((bg & 7) << 4) | (fgb & 0x1) << 3 | (fg & 7))
#define PAINT(a,b) (((b & 0xFF) << 8) | (a & 0xFF))

uint16_t * buffer_address=0;
uint32_t buffer_length = 0;
uint16_t columns=0, rows=0;
uint8_t bpp=0;
uint16_t x=0,y=0;

uint8_t foreground = VGA_COLOR_LIGHTGRAY;
uint8_t background = VGA_COLOR_BLUE;

void clear_screen(){

    for (uint32_t i = 0; i < buffer_length; i++)
    {
        buffer_address[i] = PAINT(' ',TEXTSTYLE(foreground,1,background,0));
    }
    
}

void vga_install(uint32_t _buffer_addr,uint16_t _columns, uint16_t _rows,uint8_t _bpp){
    buffer_address = (ptr_t)_buffer_addr;
    columns = _columns;
    rows = _rows;
    bpp = _bpp;
    buffer_length = columns * rows;
    clear_screen();
}

/*
 * Update cursor
 * */
void update_cursor() {

    unsigned curr_pos = y * columns + x;

    outportb(0x3D4, 14);
    outportb(0x3D5, curr_pos >> 8);
    outportb(0x3D4, 15);
    outportb(0x3D5, curr_pos);
}
void scroll(){
    for (uint16_t i = 1; i < rows; i++)
    {
        for (uint16_t j = 0; j < columns; j++)
        {
            uint32_t from = (uint32_t)PIXEL(j,(i-1));
            uint32_t to = (uint32_t)PIXEL(j,i);
            PIXEL(j,(i-1)) = PIXEL(j,i);
        }
    }

    for (uint16_t i = 0; i < columns; i++)
    {
        PIXEL(i,(rows-1)) = PAINT(' ',TEXTSTYLE(foreground,1,background,0));
    }
    
}
void print_char(char c){
    if(c == '\n'){
        y++;
    }
    else if(c == '\t')
    {
        x+=8;
    }
    else if(c == '\b'){
        x--;
        PIXEL(x,y) = PAINT(' ',TEXTSTYLE(foreground,1,background,0));
    }
    else if(c == '\r'){
        x=0;
    }
    else if(isprint(c))
    {
        PIXEL(x,y) = PAINT(c,TEXTSTYLE(foreground,1,background,0));
        x++;
    }

    if(x > columns){
        x-=columns;
        y++;
    }

    if(y>=rows){
        y -- ;
        x= 0;
        scroll();
    }

    update_cursor();
}

void print_string(string_t str){
    while (*str)
    {
        print_char(*str);
        str++;
    }
}
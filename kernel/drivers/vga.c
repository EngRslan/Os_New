#include <kernel/types.h>
#include <kernel/bits.h>
#include <kernel/system.h>
#include <ctype.h>
#include <stdlib.h>
#include <stddef.h>
#include <kernel/drivers/vga.h>
#define PIXEL(x,y)buffer_address[y*columns+x]
#define TEXTSTYLE(fg,fgb,bg,bgb)(((bgb & 0x1)<<7) | ((bg & 7) << 4) | (fgb & 0x1) << 3 | (fg & 7))
#define PAINT(a,b) (((b & 0xFF) << 8) | (a & 0xFF))
#define MIN(a,b)(a>b?b:a)
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
        x=0;
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
void csi(char* args,char command)
{
    char *endptr = NULL;
   switch (command)
   {
   case 'A': // CUU => Move the cursor up n rows
   {
        uint32_t n = strtol(args,&endptr,10);
        if(n>0){
            y -= MIN(y,n);
        }
        break;
   }
   case 'B': // CUD => Move the cursor down n rows
   {
        uint32_t n = strtol(args,&endptr,10);
        if(n>0){
            y += n;
        }
        break;
   }
   case 'C': // CUF => Move the cursor right n columns
   {

        uint32_t n = strtol(args,&endptr,10);
        if(n>0){
            x += n;
        }
        break;
   }
   case 'D': // CUB => Move the cursor Left n rows
    {
        uint32_t n = strtol(args,&endptr,10);
        if(n>0){
            x -= n;
        }
        break;
    } 
   case 'E': // CNL => Move the cursor next n lines
   {
        uint32_t n = strtol(args,&endptr,10);
        if(n==0)n=1;
        x = 0;
        y+=n;
        if(y>rows){
            uint32_t overflow = y - rows - 1;
            scroll();
            y -= overflow;
        }
        break;
   }
   case 'F': // CPL => Move the cursor Previous n lines
   {
        uint32_t n = strtol(args,&endptr,10);
        if(n==0)n=1;
        x = 0;
        y-=n;
        break;
   }
   case 'G': // CHA => Move the cursor to column n
   {
        uint32_t n = strtol(args,&endptr,10);
        if(n==0)n=1;
        x = n;
        break;
   }
   case 'H': // CUP => move the cursor to row n and column y
   {
        uint32_t n = strtol(args,&endptr,10);
        uint32_t y = strtol(endptr++,&endptr,10);

        x = n;
        y = y;

        break;
   }
   case 'f':

    break;
   case 'J': // ED => Clear Screen
                // MODE 0 => Clear from cursor to end
                // MODE 1 => Clear from Cursor to up
                // Mode 2 => Clear entire screen
    {
        uint32_t n = strtol(args,&endptr,10);
        switch (n)
        {
            case 0:
                
                break;
            case 1:
                
                break;
            case 2:
                clear_screen();
                break;
            default:
                break;
        }
        break;
    }
        
   case 'K': // EL => Clear Line
                // Mode 0 => Clear from cursor to line end
                // Mode 1 => Clear from cursor to line start
                // Mode 3 => Clear entire line
        
    break;
   case 'S':

    break;
   case 'T':

    break;
   case 'm': // SCGR=> change cursor foreground and background
                //Mode 0 => Reset Color Attributes
                //Mode 1 => Set Font Weight
                //MODE 30 => Foreground Black
                //MODE 31 => Foreground RED
                //MODE 32 => Foreground GREEN
                //MODE 33 => Foreground Yellow
                //MODE 34 => Foreground BLUE
                //MODE 35 => Foreground MAGENTA
                //MODE 36 => Foreground CYAN
                //MODE 37 => Foreground WHITE

                //MODE 40 => Background BLACK
                //MODE 41 => Background RED
                //MODE 42 => Background GREEN
                //MODE 43 => Background Yellow
                //MODE 44 => Background BLUE
                //MODE 45 => Background MAGENTA
                //MODE 46 => Background CYAN
                //MODE 47 => Background WHITE

    break;
   default:

    break;
   }
}
void print_string(string_t str){
    char *commandstr = NULL;
    char command = 0;

    while (*str)
    {

        if(*str=='\033'){
            if(*++str == '['){
                //handle commands
                commandstr = ++str;
                while (!isalpha(*++str));
                command = *str;
                *str++=0;
                csi(commandstr,command);
            }
        }else{
            print_char(*str);
            str++;
        }
    }
}
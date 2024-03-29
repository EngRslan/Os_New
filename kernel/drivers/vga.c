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
uint8_t terminalColors[]={
    [30]=VGA_COLOR_BLACK,
    [31]=VGA_COLOR_RED,
    [32]=VGA_COLOR_GREEN,
    [33]=VGA_COLOR_BROWN,
    [34]=VGA_COLOR_BLUE,
    [35]=VGA_COLOR_MAGENTA,
    [36]=VGA_COLOR_CYAN,
    [37]=VGA_COLOR_LIGHTGRAY,
    [40]=VGA_COLOR_BLACK,
    [41]=VGA_COLOR_RED,
    [42]=VGA_COLOR_GREEN,
    [43]=VGA_COLOR_BROWN,
    [44]=VGA_COLOR_BLUE,
    [45]=VGA_COLOR_MAGENTA,
    [46]=VGA_COLOR_CYAN,
    [47]=VGA_COLOR_LIGHTGRAY,
};
uint16_t * buffer_address=0;
uint32_t buffer_length = 0;
uint16_t columns=0, rows=0;
uint8_t bpp=0;
uint16_t x=0,y=0;

#define DEFAULT_VGA_FOREGROUND VGA_COLOR_LIGHTGRAY
#define DEFAULT_VGA_BACKGROUND VGA_COLOR_BLACK
#define DEFAULT_VGA_FOREGROUND_BRIGHT 1
#define DEFAULT_VGA_BACKGROUND_BRIGHT 0

uint8_t foreground = DEFAULT_VGA_FOREGROUND;
uint8_t background = DEFAULT_VGA_BACKGROUND;
uint8_t foreground_bright = DEFAULT_VGA_FOREGROUND_BRIGHT;
uint8_t background_bright = DEFAULT_VGA_BACKGROUND_BRIGHT;

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
   case 'H': // CUP => move the cursor to row n and column w
   {
        uint32_t w = strtol(args,&endptr,10);
        uint32_t n = strtol(++endptr,&endptr,10);

        x = n;
        y = w;

        break;
   }
   case 'f':
    {
        uint32_t w = strtol(args,&endptr,10);
        uint32_t n = strtol(++endptr,&endptr,10);

        x = n;
        y = w;/* code */
   }
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
                for (int r = y, c=x; r<rows; r++,c=0)
                {
                    for (; c < columns; c++)
                    {
                        PIXEL(c,r) = PAINT(' ',TEXTSTYLE(foreground,1,background,0));
                    }
                    
                }
                break;
            case 1:
                for (int r = y, c=x; r>=0; r--,c=columns)
                {
                    for (; c >= 0; c--)
                    {
                        PIXEL(c,r) = PAINT(' ',TEXTSTYLE(foreground,1,background,0));
                    }
                    
                }
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
                // Mode 2 => Clear entire line
        {
            uint32_t n = strtol(args,&endptr,10);
            switch (n)
            {
            case 0:
            {
                for (int tmpx = x; tmpx < columns; tmpx++)
                {
                    PIXEL(tmpx,y) = PAINT(' ',TEXTSTYLE(foreground,1,background,0));
                }
                
                break;
            }    
            case 1:
            {
                for (int tmpx = 0; tmpx < x; tmpx++)
                {
                    PIXEL(tmpx,y) = PAINT(' ',TEXTSTYLE(foreground,1,background,0));
                }
                break;
            }
            case 2:
            {
                for (int tmpx = 0; tmpx < columns; tmpx++)
                {
                    PIXEL(tmpx,y) = PAINT(' ',TEXTSTYLE(foreground,1,background,0));
                }
                break;
            }
                
            
            default:
                break;
            }
        }
    break;
   case 'S':

    break;
   case 'T':

    break;
   case 'm': // SCGR=> change cursor foreground and background
   {
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
                uint32_t n1 =0;
                uint32_t n2 =0;
                uint32_t n3 =0;
                uint32_t n4 =0;
                if(args != NULL){
                    n1 = strtol(args,&endptr,10);
                    n2 = strtol(++endptr,&endptr,10);
                    n3 = strtol(++endptr,&endptr,10);
                    n4 = strtol(++endptr,&endptr,10);
                }
        if(n1 == 0 && n2 == 0 && n3==0 && n4==0){
            foreground = DEFAULT_VGA_FOREGROUND;
            background = DEFAULT_VGA_BACKGROUND;
            foreground_bright = DEFAULT_VGA_FOREGROUND_BRIGHT;
            background_bright = DEFAULT_VGA_BACKGROUND_BRIGHT;
            break;
        }

        if(n1>=30 && n1<=37){
            foreground = terminalColors[n1] ;
        }

        if(n2>=30 && n2<=37){
            foreground = terminalColors[n2];
        }

        if(n1>=40 && n1<=47){
            background = terminalColors[n1];
        }

        if(n2>=40 && n2<=47){
            background = terminalColors[n2];
        }

        if(n4>=40 && n4<=47){
            background = terminalColors[n4];
        }

        if(n1 == 0||n1 == 1){
            foreground_bright = n1;
        }

        if(n3 == 0||n3 == 1){
            background_bright = n3;
        }
        break;
    }
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
                commandstr = NULL;
                if(isdigit(*++str))
                    commandstr = str;
                    else
                    str --;

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
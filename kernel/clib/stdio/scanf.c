#include <kernel/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <kernel/drivers/keyboard.h>

char keybuf=0;

void keyboard_keydown_event(keyboard_event_t event)
{
    if(event.ascii)
    {
        keybuf =  event.ascii;
    }
}

char readKey(){
    while (!keybuf) { }
    char key = keybuf;
    keybuf = 0;
    return key;
}
long read_number(int base){
    char buf[20];
    int pos = 0;
    char *ptr;
    char k = 0;
    while ((k = readKey())!='\n' && pos < 20)
    {
        if(k == '\b' && pos > 0){
            pos--;
            buf[pos]=0;
            putchar(k);
        }
        
        if((base == 10 && !isdigit(k)) || (base == 16 && !isxdigit(k))){
            continue;
        }

        putchar(k);
        buf[pos]=k;
        pos++;
    }
    buf[pos]=0x0;
    return strtol(buf, &ptr, base);
};
int vscanf(const char *format, va_list args){
    register_keyboard_event_handler(keyboard_keydown_event,KEY_DOWN_EVENT);
    char c;
    while ((c = *format++) != 0)
    {
        if(c != '%')
        {
            putchar(c);
        }else{
            c = *format++;
            switch (c)
            {
                case 'd':
                case 'x':
                case 'u':
                {
                    int * s = (int *)va_arg(args,int);
                    *s = read_number(c == 'x'?16:10);
                    break;
                }
                case 's':
                    char * s = (char *)va_arg(args,int);
                    read_string(s);
                break;
                default:
                    break;
            }
        }
    }

    return 0;
    
}
int scanf(const char *format, ...){
    va_list args;
    va_start(args,format);
    return vscanf(format,args);
}
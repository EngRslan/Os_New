#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <kernel/drivers/vga.h>

void putchar(char c){
    print_char(c);
}
void vsprintf(char *str, const char *format, va_list arg){
    int c;
    char buf[20];
    while ((c = *format++) != 0)
    {
        if (c != '%')
        {
            *str = c;
            str++;
        }
        else
        {
            char *p, *p2;
            int pad0 = 0, pad = 0;

            c = *format++;
            if (c == '0')
            {
                pad0 = 1;
                c = *format++;
            }

            if (c >= '0' && c <= '9')
            {
                pad = c - '0';
                c = *format++;
            }

            switch (c)
            {
            case 'd':
            case 'u':
            case 'x':
                itoa(buf, c, va_arg(arg,int));
                p = buf;
                goto string;
                break;

            case 's':
                p = (char *)va_arg(arg,int);
                if (!p)
                    p = "(null)";

            string:
                for (p2 = p; *p2; p2++)
                    ;
                for (; p2 < p + pad; p2++){
                    *str = pad0 ? '0' : ' ';
                    str++;
                }
                while (*p)
                {
                    *str = *p++;
                    str++;
                }
                break;
            case 'f':
                break;
            default:
                    *str =  va_arg(arg,int);
                    str++;
                break;
            }
        }
    }

    *str = 0x0;
}
void sprintf(char *str,const char *format, ...){
    va_list args;
    va_start(args,format);
    vsprintf(str,format,args);
}
void vprintf(const char *format, va_list arg)
{
    char buf[512];
    vsprintf(buf,format,arg);
    print_string(buf);
}
void printf(const char *format, ...)
{
    va_list args;
    va_start(args,format);
    vprintf(format,args);
}
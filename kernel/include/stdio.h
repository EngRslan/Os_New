#ifndef STDIO_H
#define STDIO_H
#include <stdarg.h>

void putchar(char c);
void printf(const char *format, ...);
void vsprintf(char *str, const char *format, va_list arg);
void vprintf(const char *format, va_list arg);
void sprintf(char *str,const char *format, ...);

int scanf(const char *format, ...);
#endif
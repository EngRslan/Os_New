#ifndef SERIAL_H
#define SERIAL_H
#include <kernel/types.h>

#define COM1 0x3f8

typedef unsigned short com_t;

void serial_install(com_t com);
void write_serial(com_t com,char a);
void serial_print(com_t com,string_t s);

#endif
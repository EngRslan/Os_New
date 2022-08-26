#include <kernel/drivers/serial.h>
#include <kernel/system.h>

void serial_install(com_t com){
    outportb(com + 1, 0x00);
    outportb(com + 3, 0x80);
    outportb(com + 0, 0x03);
    outportb(com + 1, 0x00);
    outportb(com + 3, 0x03);
    outportb(com + 2, 0xC7);
    outportb(com + 4, 0x0B);
}

int serial_received(com_t com) {
   return inportb(com + 5) & 1;
}

char read_serial(com_t com) {
   while (serial_received(com) == 0);
   return inportb(com);
}

// Send

int is_transmit_empty(com_t com) {
   return inportb(com + 5) & 0x20;
}

void write_serial(com_t com,char a) {
   while (is_transmit_empty(com) == 0);
   outportb(com,a);
}

void serial_print(com_t com,string_t s) {
    while (*s)
    {
        write_serial(COM1,*s);
        s++;
    }
}
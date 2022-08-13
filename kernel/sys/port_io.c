#include <kernel/system.h>


/*
 * write a bytes
 * */
void outportb(unsigned short port, unsigned char val) {
    asm volatile("outb %1, %0" : : "dN"(port), "a"(val));
}

/*
 * read a byte
 * */
unsigned char inportb(unsigned short port) {
    unsigned char ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}
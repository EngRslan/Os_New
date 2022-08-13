#ifndef SYSTEM_H
#define SYSTEM_H
typedef struct registers
{
    unsigned int ds;
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
    unsigned int int_no, err_code;
    unsigned int eip, cs, eflags, useresp, ss;
}register_t;



void outportb(unsigned short port, unsigned char val);
unsigned char inportb(unsigned short port);
#endif
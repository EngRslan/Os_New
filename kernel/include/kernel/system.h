#ifndef SYSTEM_H
#define SYSTEM_H
#include <kernel/types.h>

typedef struct registers
{
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
}register_t;



void outportb(uint16_t port, uint8_t val);
uint8_t inportb(uint16_t port);
uint16_t inports(uint16_t _port);
void outports(uint16_t _port, uint16_t _data);
uint32_t inportl(uint16_t _port);
void outportl(uint16_t _port, uint32_t _data);
void InterruptClear();
void InterruptSet();
void InterruptForceClear();
void InterruptForceSet();
void InterruptSoftClear();
#endif
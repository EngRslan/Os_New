
#include<kernel/isr.h>
#include<stdio.h>

isr_t interrupt_handlers[256];

void register_interrupt_handler(int num, isr_t handler) {
    printf("IRQ %d Registered\n", num);
    if(num < 256)
        interrupt_handlers[num] = handler;
}
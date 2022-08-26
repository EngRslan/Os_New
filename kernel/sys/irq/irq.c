
#include<kernel/isr.h>
#include<logger.h>

isr_t interrupt_handlers[256];

void register_interrupt_handler(int num, isr_t handler) {
    if(num < 256)
        interrupt_handlers[num] = handler;
    log_trace("IRQ %d Registerd Successfully",num);
}
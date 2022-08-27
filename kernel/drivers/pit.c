/*
    Programable Interval Timer
*/

#include <kernel/types.h>
#include <kernel/system.h>
#include <kernel/drivers/pit.h>
#include <kernel/isr.h>
#include <logger.h>

#define INPUT_CLOCK_FREQUENCY  1193182
#define CHANNEL_0_DATA_PORT 0x40
#define COMMAND_REGISTER_PORT 0x43
void interrupt_handler(register_t * reg);
uint64_t millis = 0;
void pit_install(){
    /*
    Bits         Usage
    6 and 7      Select channel :
                    0 0 = Channel 0
                    0 1 = Channel 1
                    1 0 = Channel 2
                    1 1 = Read-back command (8254 only)
    4 and 5      Access mode :
                    0 0 = Latch count value command
                    0 1 = Access mode: lobyte only
                    1 0 = Access mode: hibyte only
                    1 1 = Access mode: lobyte/hibyte
    1 to 3       Operating mode :
                    0 0 0 = Mode 0 (interrupt on terminal count)
                    0 0 1 = Mode 1 (hardware re-triggerable one-shot)
                    0 1 0 = Mode 2 (rate generator)
                    0 1 1 = Mode 3 (square wave generator)
                    1 0 0 = Mode 4 (software triggered strobe)
                    1 0 1 = Mode 5 (hardware triggered strobe)
                    1 1 0 = Mode 2 (rate generator, same as 010b)
                    1 1 1 = Mode 3 (square wave generator, same as 011b)
    0            BCD/Binary mode: 0 = 16-bit binary, 1 = four-digit BCD
    */
    outportb(COMMAND_REGISTER_PORT,0b00110110);

    uint16_t freq = INPUT_CLOCK_FREQUENCY / 1000;
    outportb(CHANNEL_0_DATA_PORT,freq & 0xFF); //LOW BYTE
    outportb(CHANNEL_0_DATA_PORT,(freq >> 8) & 0xFF); // HIGH BYTE

    register_interrupt_handler(IRQ_BASE + IRQ0_Timer,interrupt_handler);
}

void interrupt_handler(register_t * reg){
    millis+=1;
    //log_trace("seconds = %d",millis);
    if((millis % 1000)==0){
        log_trace("seconds = %d",millis/1000);
    }
}
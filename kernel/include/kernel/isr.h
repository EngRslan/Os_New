#ifndef ISR_H
#define ISR_H
#include <kernel/system.h>
// Interrupt Service Routine function prototype
typedef void (*isr_t)(register_t * );
extern isr_t interrupt_handlers[256];

// Some IRQ constants
#define IRQ_BASE                0x20
#define IRQ0_Timer              0x00
#define IRQ1_Keyboard           0x01
#define IRQ2_CASCADE            0x02
#define IRQ3_SERIAL_PORT2       0x03
#define IRQ4_SERIAL_PORT1       0x04
#define IRQ5_RESERVED           0x05
#define IRQ6_DISKETTE_DRIVE     0x06
#define IRQ7_PARALLEL_PORT      0x07
#define IRQ8_CMOS_CLOCK         0x08
#define IRQ9_CGA                0x09
#define IRQ10_RESERVED          0x0A
#define IRQ11_RESERVED          0x0B
#define IRQ12_AUXILIARY         0x0C
#define IRQ13_FPU               0x0D
#define IRQ14_HARD_DISK         0x0E
#define IRQ15_RESERVED          0x0F

void register_interrupt_handler(int num, isr_t handler);

#endif
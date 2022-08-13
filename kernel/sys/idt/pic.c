#include <kernel/pic.h>
#include <kernel/system.h>

void pic_install() {
    // ICW1
    outportb(PIC1_COMMAND, ICW1);
    outportb(PIC2_COMMAND, ICW1);

    // ICW2, irq 0 to 7 is mapped to 0x20 to 0x27, irq 8 to F is mapped to 28 to 2F
    outportb(PIC1_DATA, 0x20);
    outportb(PIC2_DATA, 0x28);

    // ICW3, connect master pic with slave pic
    outportb(PIC1_DATA, 0x4);
    outportb(PIC2_DATA, 0x2);

    // ICW4, set x86 mode
    outportb(PIC1_DATA, 1);
    outportb(PIC2_DATA, 1);

    // clear the mask register
    outportb(PIC1_DATA, 0);
    outportb(PIC2_DATA, 0);
}


/*
 * Tell PIC interrupt is handled
 * */
void irq_ack(unsigned char irq) {
    if(irq >= 0x28)
        outportb(PIC2, PIC_EOI);
    outportb(PIC1, PIC_EOI);
}
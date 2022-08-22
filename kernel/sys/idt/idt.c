#include <kernel/idt.h>
#include <kernel/isr.h>
#include <kernel/pic.h>
#include <kernel/system.h>
#include <string.h>
#include <stdio.h>

// Extern asm functions
extern void idt_flush(unsigned int ptr);

idt_entry_t idt_entries[NUM_IDT_ENTRIES];
idt_ptr_t idt_ptr;

char *exception_messages[32] =
{
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",
    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

void idt_install(){
    memset(idt_entries, 0, sizeof(idt_entries));
    idt_ptr.base = (unsigned int)idt_entries;
    idt_ptr.limit = sizeof(idt_entries) - 1;

    pic_install();

    idt_set_entry( 0, (unsigned int)exception0 , 0x08, 0x8E);
    idt_set_entry( 1, (unsigned int)exception1 , 0x08, 0x8E);
    idt_set_entry( 2, (unsigned int)exception2 , 0x08, 0x8E);
    idt_set_entry( 3, (unsigned int)exception3 , 0x08, 0x8E);
    idt_set_entry( 4, (unsigned int)exception4 , 0x08, 0x8E);
    idt_set_entry( 5, (unsigned int)exception5 , 0x08, 0x8E);
    idt_set_entry( 6, (unsigned int)exception6 , 0x08, 0x8E);
    idt_set_entry( 7, (unsigned int)exception7 , 0x08, 0x8E);
    idt_set_entry( 8, (unsigned int)exception8 , 0x08, 0x8E);
    idt_set_entry( 9, (unsigned int)exception9 , 0x08, 0x8E);
    idt_set_entry(10, (unsigned int)exception10, 0x08, 0x8E);
    idt_set_entry(11, (unsigned int)exception11, 0x08, 0x8E);
    idt_set_entry(12, (unsigned int)exception12, 0x08, 0x8E);
    idt_set_entry(13, (unsigned int)exception13, 0x08, 0x8E);
    idt_set_entry(14, (unsigned int)exception14, 0x08, 0x8E);
    idt_set_entry(15, (unsigned int)exception15, 0x08, 0x8E);
    idt_set_entry(16, (unsigned int)exception16, 0x08, 0x8E);
    idt_set_entry(17, (unsigned int)exception17, 0x08, 0x8E);
    idt_set_entry(18, (unsigned int)exception18, 0x08, 0x8E);
    idt_set_entry(19, (unsigned int)exception19, 0x08, 0x8E);
    idt_set_entry(20, (unsigned int)exception20, 0x08, 0x8E);
    idt_set_entry(21, (unsigned int)exception21, 0x08, 0x8E);
    idt_set_entry(22, (unsigned int)exception22, 0x08, 0x8E);
    idt_set_entry(23, (unsigned int)exception23, 0x08, 0x8E);
    idt_set_entry(24, (unsigned int)exception24, 0x08, 0x8E);
    idt_set_entry(25, (unsigned int)exception25, 0x08, 0x8E);
    idt_set_entry(26, (unsigned int)exception26, 0x08, 0x8E);
    idt_set_entry(27, (unsigned int)exception27, 0x08, 0x8E);
    idt_set_entry(28, (unsigned int)exception28, 0x08, 0x8E);
    idt_set_entry(29, (unsigned int)exception29, 0x08, 0x8E);
    idt_set_entry(30, (unsigned int)exception30, 0x08, 0x8E);
    idt_set_entry(31, (unsigned int)exception31, 0x08, 0x8E);
    idt_set_entry(32, (unsigned int)irq0, 0x08, 0x8E);
    idt_set_entry(33, (unsigned int)irq1, 0x08, 0x8E);
    idt_set_entry(34, (unsigned int)irq2, 0x08, 0x8E);
    idt_set_entry(35, (unsigned int)irq3, 0x08, 0x8E);
    idt_set_entry(36, (unsigned int)irq4, 0x08, 0x8E);
    idt_set_entry(37, (unsigned int)irq5, 0x08, 0x8E);
    idt_set_entry(38, (unsigned int)irq6, 0x08, 0x8E);
    idt_set_entry(39, (unsigned int)irq7, 0x08, 0x8E);
    idt_set_entry(40, (unsigned int)irq8, 0x08, 0x8E);
    idt_set_entry(41, (unsigned int)irq9, 0x08, 0x8E);
    idt_set_entry(42, (unsigned int)irq10, 0x08, 0x8E);
    idt_set_entry(43, (unsigned int)irq11, 0x08, 0x8E);
    idt_set_entry(44, (unsigned int)irq12, 0x08, 0x8E);
    idt_set_entry(45, (unsigned int)irq13, 0x08, 0x8E);
    idt_set_entry(46, (unsigned int)irq14, 0x08, 0x8E);
    idt_set_entry(47, (unsigned int)irq15, 0x08, 0x8E);
    idt_set_entry(128, (unsigned int)exception128, 0x08, 0x8E);
    
    idt_flush((unsigned int)&(idt_ptr));
    asm volatile("sti");
}

void idt_set_entry(int index, unsigned int base, unsigned short sel, unsigned char flags) {
    idt_entry_t * this = &idt_entries[index];
    this->base_lo = base & 0xFFFF;
    this->base_hi = (base >> 16) & 0xFFFF;
    this->always0 = 0;
    this->sel = sel;
    this->flags = flags | 0x60;
}

void exception_handler(register_t reg) {
    if(reg.int_no < 32) {
        if(reg.int_no == 0xe){
            unsigned int bad_address=0;
            __asm__ __volatile__("movl %%cr2, %0":"=r"(bad_address));
            printf("EXCEPTION: %s (err code is 0x%x) (Bad address is 0x%x)\n", exception_messages[reg.int_no], reg.err_code, bad_address);

        }else{
            printf("EXCEPTION: %s (err code is 0x%x)\n", exception_messages[reg.int_no], reg.err_code);
        }
        
        for(;;);
    }
    if(interrupt_handlers[reg.int_no] != NULL) {
         isr_t handler = interrupt_handlers[reg.int_no];
         handler(&reg);
    }

}

void irq_handler(register_t * reg) {
    if(interrupt_handlers[reg->int_no] != NULL) {
        isr_t handler = interrupt_handlers[reg->int_no];
        handler(reg);
    }
    irq_ack(reg->int_no);
}


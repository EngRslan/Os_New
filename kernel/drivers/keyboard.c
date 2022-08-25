#include <kernel/types.h>
#include <kernel/isr.h>

void keyboard_handler(register_t * r){
    int32_t i, scancode;
    for(i = 1000; i > 0; i++) {
        // Check if scan code is ready
        if((inportb(0x64) & 1) == 0) continue;
        // Get the scan code
        scancode = inportb(0x60);
        break;
    }
    if(i > 0) {
        if(scancode & 0x80) {
            // Key release
        }
        else {
            // Key down
            
        }
    }
}
void keyboard_install(){
    register_interrupt_handler(IRQ_BASE + 1,keyboard_handler);
}
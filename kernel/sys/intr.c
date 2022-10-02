#include <kernel/system.h>

static int32_t intrRefs = 0;
void InterruptClear(){
    intrRefs ++;
    if(intrRefs > 1){
        return;
    }


    __asm__ __volatile__("cli");
}

void InterruptSet(){
    intrRefs --;
    if(intrRefs > 0){
        return;
    }

    __asm__ __volatile__("sti");

}

void InterruptForceClear(){
    __asm__ __volatile__("cli");
}

void InterruptForceSet(){
    __asm__ __volatile__("sti");
}

void InterruptSoftClear(){
    if(intrRefs > 0){
        __asm__ __volatile__("cli");
        return;
    }

        __asm__ __volatile__("sti");
}
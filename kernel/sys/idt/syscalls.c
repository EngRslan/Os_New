#include <kernel/isr.h>
#include <kernel/system.h>
#include <stdio.h>

int syscall_1(unsigned int ebx,unsigned int ecx,unsigned int edx,unsigned int esi)
{
    printf("syscall 0 called");
    return 120;
}

void * syscall_table[1] = {
    syscall_1
};

void syscall_dispatcher(register_t * regs) {
    if(regs->eax >= 1) return;
    void * system_api = syscall_table[regs->eax];
    int ret;
    asm volatile (" \
     pushl %1; \
     pushl %2; \
     pushl %3; \
     pushl %4; \
     pushl %5; \
     call *%6; \
     popl %%ebx; \
     popl %%ebx; \
     popl %%ebx; \
     popl %%ebx; \
     popl %%ebx; \
     " : "=a" (ret) : "r" (regs->edi), "r" (regs->esi), "r" (regs->edx), "r" (regs->ecx), "r" (regs->ebx), "r" (system_api));

    // I don't beleive this would set eax to return value ?
    regs->eax = ret;
}

void syscalls_install(){
    register_interrupt_handler(0x80, syscall_dispatcher);
}
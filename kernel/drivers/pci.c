#include <kernel/drivers/pci.h>
#include <kernel/system.h>


uint32_t read_pci(uint8_t bus, uint8_t device,uint8_t func,uint8_t offset){
    uint32_t address = 0;
    uint32_t lbus = (uint32_t)bus;
    uint32_t ldevice = (uint32_t)device;
    uint32_t lfunc = (uint32_t)func;
    uint32_t temp = 0;

    address = (uint32_t)((lbus<<16)|(device<<11)|(lfunc<<8)|(offset & 0xFC) | ((uint32_t)0x80000000));
    outportl(0xCF8+1,address);
    uint32_t inport = inportl(0xCFC) ;
    temp = (inport >> ((offset & 2)*8)) & 0xFFFF;
    return temp;
}
#include <kernel/drivers/pci.h>
#include <kernel/system.h>


// uint32_t read_pci(uint8_t bus, uint8_t device,uint8_t func,uint8_t offset){
//     uint32_t address = 0;
//     uint32_t lbus = (uint32_t)bus;
//     uint32_t ldevice = (uint32_t)device;
//     uint32_t lfunc = (uint32_t)func;
//     uint32_t temp = 0;

//     address = (uint32_t)((lbus<<16)|(device<<11)|(lfunc<<8)|(offset & 0xFC) | ((uint32_t)0x80000000));
//     outportl(0xCF8,address);
//     uint32_t inport = inportl(0xCFC) ;
//     temp = (inport >> ((offset & 2)*8)) & 0xFFFF;
//     return temp;
// }


void read_pci(intptr_t buffer, uint8_t bus, uint8_t device,uint8_t func){
    uint32_t address = 0;
    uint32_t lbus = (uint32_t)bus;
    uint32_t ldevice = (uint32_t)device;
    uint32_t lfunc = (uint32_t)func;
    uint32_t temp = 0;
    intptr_t buffer_i = buffer;
    for (uint32_t i = 0; i < 4; i++)
    {
        address = (uint32_t)((lbus<<16)|(device<<11)|(lfunc<<8)|((i*4) & 0xFC) | ((uint32_t)0x80000000));
        outportl(0xCF8,address);
        *buffer_i = inportl(0xCFC) ;
        if(*buffer_i == 0xFFFFFFFF){

            return 0;
        }
        buffer_i++;
    }
    
    // address = (uint32_t)((lbus<<16)|(device<<11)|(lfunc<<8)|(offset & 0xFC) | ((uint32_t)0x80000000));
    // outportl(0xCF8,address);
    // uint32_t inport = inportl(0xCFC) ;
    // temp = (inport >> ((offset & 2)*8)) & 0xFFFF;
    // return temp;
    return 0;
}
uint32_t pci_read(pci_command_t command){
    command.always_zero =0;
    command.enable =1;
    outportl(0xCF8,command.bits);
    return inportl(0xCFC);
}

pci_header_type_t pci_read_header_type(uint32_t bus,uint32_t device,uint32_t function){
    pci_command_t command = {
        .bus = bus,
        .device = device,
        .function = function,
        .offset = PCI_HEADER_TYPE
    };

    pci_header_type_t header = {
        .bits = (uint8_t)pci_read(command)
    };

    return header;
}


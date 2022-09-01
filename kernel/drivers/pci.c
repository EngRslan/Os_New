#include <kernel/drivers/pci.h>
#include <kernel/system.h>
#include <kernel/datastruct/list.h>
#include <kernel/datastruct/gtree.h>
#include <kernel/mem/kheap.h>
#include <stddef.h>
uint32_t pci_read(pci_command_t command);
int32_t pci_read_config_block(const intptr_t buffer, pci_command_t cmd);

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

// void read_pci(intptr_t buffer, uint8_t bus, uint8_t device,uint8_t func){
//     uint32_t address = 0;
//     uint32_t lbus = (uint32_t)bus;
//     uint32_t ldevice = (uint32_t)device;
//     uint32_t lfunc = (uint32_t)func;
//     uint32_t temp = 0;
//     intptr_t buffer_i = buffer;
//     for (uint32_t i = 0; i < 4; i++)
//     {
//         address = (uint32_t)((lbus<<16)|(device<<11)|(lfunc<<8)|((i*4) & 0xFC) | ((uint32_t)0x80000000));
//         outportl(0xCF8,address);
//         *buffer_i = inportl(0xCFC) ;
//         if(*buffer_i == 0xFFFFFFFF){

//             return 0;
//         }
//         buffer_i++;
//     }

//     // address = (uint32_t)((lbus<<16)|(device<<11)|(lfunc<<8)|(offset & 0xFC) | ((uint32_t)0x80000000));
//     // outportl(0xCF8,address);
//     // uint32_t inport = inportl(0xCFC) ;
//     // temp = (inport >> ((offset & 2)*8)) & 0xFFFF;
//     // return temp;
//     return 0;
// }
// void pci_scan_device_functions(const list_t * list,const uint32_t bus, const uint32_t device){
//     pci_command_t cmd ={
//         .always_zero =0,
//         .bus = bus,
//         .device = device,
//         .enable = 1,
//         .function = 0,
//         .offset = 0,
//     };
//     for (uint32_t function = 0; function < PCI_MAX_FUNCTIONS_PER_DEVICE; function++)
//     {
//         cmd.function = function;
//         if(pci_read_header_type(cmd).bits == 0xFF){
//             continue;
//         }
//         struct pci_config_common_t * config_buffer = (struct pci_config_common_t *)kmalloc(PCI_CONFIG_ACCESS_SIZE);
//         pci_read_config_block(config_buffer,cmd);
//         list_add_item(list,(ptr_t *)config_buffer);
//     }

// }

// uint32_t pci_scan_devices(const list_t * list,const uint32_t bus, const uint32_t device){
//     pci_command_t cmd ={
//         .always_zero =0,
//         .bus = bus,
//         .device = device,
//         .enable = 1,
//         .function = 0,
//         .offset = 0,
//     };
//     uint32_t found = 0;
//     for (uint32_t function = 0; function < PCI_MAX_FUNCTIONS_PER_DEVICE; function++)
//     {
//         cmd.function = function;
//         if(pci_read_header_type(cmd).bits == 0xFF){
//             continue;
//         }
//         struct pci_config_common_t * config_buffer = (struct pci_config_common_t *)kmalloc(PCI_CONFIG_ACCESS_SIZE);
//         pci_read_config_block(config_buffer,cmd);
//         list_add_item(list,(ptr_t *)config_buffer);
//         found ++;
//     }

//     return found;

// }
// uint32_t pci_scan_bus(gtree_node_t * tree_node,const int bus){
//     pci_command_t cmd ={
//         .always_zero =0,
//         .bus = bus,
//         .enable = 1,
//         .function = 0,
//         .offset = 0,
//     };
//     uint32_t found = 0;
//     for (uint32_t device = 0; device < PCI_MAX_DEVICES_PER_BUS; device++)
//     {uint32_t s

//         uint32_t devices_on_bus = pci_scan_devices(tr,bus,device);
//         found++;
//     }

// }

int32_t pci_read_config_block(const intptr_t buffer, pci_command_t cmd)
{
    intptr_t _buffer = buffer;
    for (uint32_t offset = 0; offset <= PCI_CONFIG_ACCESS_SIZE; offset += 4)
    {
        cmd.offset = offset;
        uint32_t buf = pci_read(cmd);
        if (buf == 0xFFFFFFFF)
        {
            return 1;
        }
        *_buffer = buf;
        _buffer++;
    }

    return 0;
}
uint32_t pci_read(pci_command_t command)
{
    command.always_zero = 0;
    command.enable = 1;
    command.offset &= 0xFC;
    command.offset >>= 2;
    outportl(0xCF8, command.bits);
    return inportl(0xCFC);
}
pci_header_type_t pci_read_header_type(uint32_t bus, uint32_t device, uint32_t function)
{
    pci_command_t command = {
        .bus = bus,
        .device = device,
        .function = function,
        .offset = PCI_HEADER_TYPE};
    pci_header_type_t header = {
        .bits = (uint8_t)pci_read(command)};

    return header;
}

// void pci_scan(gtree_t *tree)
// {

//     for (uint32_t bus = 0; bus < PCI_MAX_BUSES; bus++)
//     {
//         gtree_node_t *bus_node = NULL;
//         for (uint32_t device = 0; device < PCI_MAX_DEVICES_PER_BUS; device++)
//         {
//             gtree_node_t *device_node = NULL;
//             pci_header_type_t header = pci_read_header_type(bus, device, 0);

//             if (header.bits == 0xFF)
//             {
//                 continue;
//             }

//             pci_config_common_t *config = (pci_config_common_t *)kmalloc(PCI_CONFIG_ACCESS_SIZE);
//             pci_command_t cmd = {
//                 .bus = bus,
//                 .device = device,
//                 .function = 0,
//             };

//             pci_read_config_block((intptr_t *)config, cmd);

//             if (!bus_node)
//             {
//                 bus_node = gtree_create_child(tree->root, bus);
//             }

//             device_node = gtree_create_child(bus_node, device);

//             gtree_node_t *function_node = gtree_create_child(device_node, (uint32_t)config);

//             if (!header.is_multifunction)
//             {
//                 continue;
//             }

//             for (uint32_t function = 0; function < PCI_MAX_FUNCTIONS_PER_DEVICE; function++)
//             {
//                 if (pci_read_header_type(bus, device, function).bits != 0xFF)
//                 {
//                     cmd.function = function;
//                     pci_read_config_block((intptr_t)config, cmd);
//                 }
//             }
//         }
//     }
// }

void pci_scan_list(list_t *list)
{
    for (uint32_t bus = 0; bus < PCI_MAX_BUSES; bus++)
    {
        for (uint32_t device = 0; device < PCI_MAX_DEVICES_PER_BUS; device++)
        {
            for (uint32_t function = 0; function < PCI_MAX_FUNCTIONS_PER_DEVICE; function++)
            {
                pci_header_type_t header = pci_read_header_type(bus, device, function);

                if (header.bits == 0xFF)
                {
                    continue;
                }

                ptr_t * config = kmalloc(PCI_CONFIG_ACCESS_SIZE);
                pci_command_t cmd = {
                    .bus = bus,
                    .device = device,
                    .function = function,
                };

                pci_read_config_block((intptr_t)config, cmd);
                pci_config_t *config_st = kmalloc(sizeof(pci_config_t));
                config_st->bus = bus;
                config_st->device = device;
                config_st->config = (pci_config_common_t *)config;
                config_st->header = header;
                config_st->function = function;
                list_add_item(list, (ptr_t)config_st);
            }
        }
    }
}
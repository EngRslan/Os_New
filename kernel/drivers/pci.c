#include <kernel/drivers/pci.h>
#include <kernel/system.h>
#include <kernel/datastruct/list.h>
#include <kernel/datastruct/gtree.h>
#include <kernel/mem/kheap.h>
#include <logger.h>
#include <stddef.h>

uint32_t pci_read(pci_command_t command);
int32_t pci_read_config_block(const intptr_t buffer, pci_command_t cmd);
list_t * pci_list;

int32_t pci_read_config_block(const intptr_t buffer, pci_command_t cmd)
{
    intptr_t _buffer = buffer;
    for (uint32_t offset = 0; offset < PCI_CONFIG_ACCESS_SIZE; offset += 4)
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
void pci_writel(uint8_t bus,uint8_t device,uint8_t func, uint32_t offset, uint32_t value){
    pci_command_t command = {
        .always_zero = 0,
        .bus = bus,
        .device = device,
        .enable = 1,
        .function = func,
        .offset=offset
    };
    command.offset &= 0xFC;
    command.offset >>= 2;

    outportl(0xCF8, command.bits);
    outportl(0xCFC, command.bits);
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
                log_trace("%s Found (bus:%d, slot:%d, function: %d) . Vendor:0x%x, id:0x%x, class:0x%x, subclass:0x%x",header.type==0?"Device":header.type==1?"Bridge":header.type==2?"CardBus":"Unknown Device",
                    bus,device,function,config_st->config->vendor_id,config_st->config->device_id,config_st->config->class_code,config_st->config->sub_class);
                list_add_item(list, (ptr_t)config_st);
            }
        }
    }
}

void pci_install(){
    pci_list = list_create();
    pci_scan_list(pci_list);
}

pci_config_t * pci_get_device(uint8_t class,uint8_t subclass){
    foreach(item,pci_list){
        pci_config_t * cfg = (pci_config_t *)item->value_ptr;
        if(cfg->config->class_code == class && cfg->config->sub_class == subclass){
            return cfg;
        }
    }
    return NULL;
}

pci_device_config_t * pci_get_device_config(uint8_t class,uint8_t subclass){
    pci_config_t * pci_dev = pci_get_device(class,subclass);
    if(pci_dev){
        return (pci_device_config_t *)pci_dev->config;
    }
    return NULL;
}


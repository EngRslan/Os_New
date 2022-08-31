#ifndef PCI_H
#define PCI_H
#include <kernel/types.h>
// Offset
#define PCI_VENDOR_ID            0x00
#define PCI_DEVICE_ID            0x02
#define PCI_COMMAND              0x04
#define PCI_STATUS               0x06
#define PCI_REVISION_ID          0x08
#define PCI_PROG_IF              0x09
#define PCI_SUBCLASS             0x0a
#define PCI_CLASS                0x0b
#define PCI_CACHE_LINE_SIZE      0x0c
#define PCI_LATENCY_TIMER        0x0d
#define PCI_HEADER_TYPE          0x0e
#define PCI_BIST                 0x0f
#define PCI_BAR0                 0x10
#define PCI_BAR1                 0x14
#define PCI_BAR2                 0x18
#define PCI_BAR3                 0x1C
#define PCI_BAR4                 0x20
#define PCI_BAR5                 0x24
#define PCI_INTERRUPT_LINE       0x3C
#define PCI_SECONDARY_BUS        0x09

#define PCI_MAX_BUSES                   0x100
#define PCI_MAX_DEVICES_PER_BUS         0x20
#define PCI_MAX_FUNCTIONS_PER_DEVICE    0x8

typedef union pci_command
{
    uint32_t bits;
    struct 
    {
        uint32_t always_zero    :2;
        uint32_t offset         :6;
        uint32_t function       :3;
        uint32_t device         :5;
        uint32_t bus            :8;   
        uint32_t res            :7;
        uint32_t enable         :1
    };
    
    
}__attribute__((packed)) pci_command_t;

typedef union pci_header_type
{
    uint8_t bits;
    struct
    {
        uint8_t type :7;
        uint8_t is_multifunction :1
    };
    
}__attribute__((packed)) pci_header_type_t;

struct pci_config_space_0
{
    uint16_t vendor_id, device_id;
    uint16_t command, status;
    uint8_t revision_id, prog_if, sub_class, class_code;
    uint8_t cach_line_size, latency_timer, header_type, bist;
    uint32_t BAR0;
    uint32_t BAR1;
    uint32_t BAR2;
    uint32_t BAR3;
    uint32_t BAR4;
    uint32_t BAR5;
    uint32_t cardbus_cis_pointer;
    uint16_t subsystem_vendor_id, subsystem_id;
    uint32_t expansion_rom_base_address;
    uint8_t capabilities_pointer;
    uint16_t _;
    uint8_t __;
    uint32_t ___;
    uint8_t interrupt_line, interrupt_pin, min_grant, max_latency;
}__attribute__((packed));

struct pci_config_space_1
{
    uint16_t vendor_id, device_id;
    uint16_t command, status;
    uint8_t revision_id, prog_if, sub_class, class_code;
    uint8_t cach_line_size, latency_timer, header_type, bist;
    uint32_t BAR0;
    uint32_t BAR1;
    uint8_t primary_bus_number, secondary_bus_number, subordinate_bus_number, secondary_latency_timer;
    uint8_t io_base, io_limit;
    uint16_t secondary_status;
    uint16_t memory_base, memory_limit;
    uint16_t prefetchable_memory_base, prefetchable_memory_limit;
    uint32_t prefetchable_base_upper;
    uint32_t prefetchable_limit_upper;
    uint16_t io_base_high, io_limit_high;
    uint8_t capabilities_pointer;
    uint16_t _;
    uint8_t __;
    uint32_t expansion_rom_base_address;
    uint8_t interrupt_line, interrupt_pin;
    uint16_t bridge_control;
}__attribute__((packed));

uint32_t pci_read(pci_command_t command);
pci_header_type_t pci_read_header_type(uint32_t bus,uint32_t device,uint32_t function);

#endif
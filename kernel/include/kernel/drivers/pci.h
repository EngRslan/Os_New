#ifndef PCI_H
#define PCI_H
#include <kernel/types.h>

typedef union pci_device_request
{
    uint32_t data;
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
    
} pci_device_request_t;

uint32_t read_pci(uint8_t bus, uint8_t device,uint8_t func,uint8_t offset);

#endif
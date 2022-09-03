#include <kernel/drivers/ide.h>
#include <kernel/drivers/pci.h>
#include <kernel/mem/vmm.h>
#include <kernel/mem/kheap.h>
#include <kernel/filesystems/vfs.h>
#include <kernel/system.h>
#include <logger.h>
#include <string.h>
#include <stddef.h>


pci_device_config_t * pci_config;
ide_device_t primary_master = { 
    .slave = 0 , 
    .mem_Buffer = (uint8_t *)GET_VIRTUAL_ADDRESS(0x300,0x3fa),
    .data_addr = ATA_DATA(0x1F0)
};
ide_device_t primary_slave = { 
    .slave = 1 , 
    .mem_Buffer= (uint8_t *)GET_VIRTUAL_ADDRESS(0x300,0x3fb),
    .data_addr = ATA_DATA(0x1F0)
};
ide_device_t secondary_master = { 
    .slave = 0 , 
    .mem_Buffer= (uint8_t *)GET_VIRTUAL_ADDRESS(0x300,0x3fc),
    .data_addr = ATA_DATA(0x170)
};
ide_device_t secondary_slave = { 
    .slave = 1 , 
    .mem_Buffer= (uint8_t *)GET_VIRTUAL_ADDRESS(0x300,0x3fd),
    .data_addr = ATA_DATA(0x170)
};

void ide_set_device(ide_device_t * device,int32_t primary){
    device->prdt = (prdt_t *)kmalloc(sizeof(prdt_t));
    memset((void *)device->prdt,0,sizeof(prdt_t));
    device->prdt_phys = (uint8_t *)virtual2physical((v_addr_t)device->prdt);
    kallocate_page((v_addr_t)device->mem_Buffer);
    memset((void *)device->mem_Buffer,0,PAGE_SIZE);
    device->prdt->buffer_phys=virtual2physical((v_addr_t)device->mem_Buffer);
    device->prdt->transfer_size=2048;
    device->prdt->mark_end = 0x8000;

    device->feature_addr = ATA_FEATURES(device->data_addr);
    device->sector_addr = ATA_SECTOR_COUNT(device->data_addr);
    device->addr1 = ATA_ADDRESS1(device->data_addr);
    device->addr2 = ATA_ADDRESS2(device->data_addr);
    device->addr3 = ATA_ADDRESS3(device->data_addr);
    device->drive = ATA_DRIVE_SELECT(device->data_addr);
    device->command = ATA_COMMAND(device->data_addr);
    device->dcr_addr = ATA_DCR(device->data_addr);

    device->BMR_COMMAND = pci_config->BAR4 & 0xFFFFFFFC;
    device->BMR_STATUS = device->BMR_COMMAND + 2;
    device->BMR_prdt = device->BMR_COMMAND + 4;

    memset(device->mountpoint,0,32);
    strcpy(device->mountpoint,"/dev/hd");

    device->mountpoint[strlen(device->mountpoint)] = 'a'+ (((!primary) << 1) | device->slave);
}
void io_wait(ide_device_t * dev) {
    inportb(dev->dcr_addr);
    inportb(dev->dcr_addr);
    inportb(dev->dcr_addr);
    inportb(dev->dcr_addr);
}
void bus_reset(ide_device_t * device){
    outportb(device->dcr_addr,0x4);
    io_wait(device);
    outportb(device->dcr_addr, 0x0);
}
void ide_device_detect(ide_device_t * device,int primary){
    ide_set_device(device,primary);
    bus_reset(device);
    outportb(device->drive, (0xA + device->slave) << 4);
    outportb(device->sector_addr, 0);
    outportb(device->addr1, 0);
    outportb(device->addr2, 0);
    outportb(device->addr3, 0);
    outportb(device->command, 0xEC);
    if(!inportb(device->command)) {
        log_warning("No Device Connected at %s",device->mountpoint);
        return;
    }

    uint8_t sec_count = inportb(device->sector_addr);
    uint8_t lba_lo = inportb(device->addr1);
    uint8_t lba_mid = inportb(device->addr2);
    uint8_t lba_hi = inportb(device->addr3);
    if(sec_count == 0 && lba_lo == 0 && lba_mid==0 && lba_hi == 0) {
        log_information("ATA %s Driver Found",device->mountpoint);
    }else if(sec_count == 1 && lba_lo == 1 && lba_mid==0x14 && lba_hi == 0xEB){
        log_information("ATAPI %s Driver Found",device->mountpoint);
    }
    else{
        log_information("Unknown Driver Found");
        return;
    }


}
void ide_install(){
    pci_config = pci_get_device_config(0x1,0x1);
    if(pci_config == NULL){
        log_error("IDE Install Failed. There is no ide controller found");
        return;
    }
    ide_device_detect(&primary_master,1);
    ide_device_detect(&primary_slave,1);
    ide_device_detect(&secondary_master,0);
    ide_device_detect(&secondary_slave,0);
    log_information("IDE Install succeded");
}
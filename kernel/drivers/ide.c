#include <kernel/drivers/ide.h>
#include <kernel/drivers/pci.h>
#include <kernel/mem/vmm.h>
#include <kernel/mem/pmm.h>
#include <kernel/mem/kheap.h>
#include <kernel/filesystems/fs.h>
#include <kernel/filesystems/vfs.h>
#include <kernel/system.h>
#include <kernel/bits.h>
#include <kernel/isr.h>
#include <logger.h>
#include <string.h>
#include <stddef.h>
#define VIRTUAL_DMA_ADDRESS GET_VIRTUAL_ADDRESS(0x300,0x3FD)

struct ide_channel channels[2];
struct ide_device devices[4];

uint8_t ide_buffer[0x800] = {0};
static volatile uint8_t ide_irq0_invoked = 0;
static volatile uint8_t ide_irq1_invoked = 0;
static int8_t atapi_packet[12] = {0xA8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t is_dma_support = 0;

void ide_write(uint8_t channel, uint8_t reg, uint8_t data)
{
    if (reg >= ATA_REG_SECTORCOUNT1 && reg <= ATA_REG_LBA5)
    {
        ide_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
    }

    if (reg <= ATA_REG_STATUS)
    {
        outportb(channels[channel].base + reg, data);
    }
    else if (reg <= ATA_REG_LBA5)
    {
        outportb(channels[channel].base + reg - 0x06, data);
    }
    else if (reg <= ATA_REG_DEVADDRESS)
    {
        outportb(channels[channel].ctrl + reg - 0x0C, data);
    }
    else if (reg < 0x16)
    {
        outportb(channels[channel].bmide + reg - 0x0E, data);
    }

    if (reg >= ATA_REG_SECTORCOUNT1 && reg <= ATA_REG_LBA5)
    {
        ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN);
    }
}
uint8_t ide_read(uint8_t channel, uint8_t reg)
{
    if (reg >= ATA_REG_SECTORCOUNT1 && reg <= ATA_REG_LBA5)
    {
        ide_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
    }

    uint8_t result;
    if (reg <= ATA_REG_STATUS)
    {
        result = inportb(channels[channel].base + reg);
    }
    else if (reg <= ATA_REG_LBA5)
    {
        result = inportb(channels[channel].base + reg - 0x06);
    }
    else if (reg <= ATA_REG_DEVADDRESS)
    {
        result = inportb(channels[channel].ctrl + reg - 0x0C);
    }
    else if (reg < 0x16)
    {
        result = inportb(channels[channel].bmide + reg - 0x0E);
    }

    if (reg >= ATA_REG_SECTORCOUNT1 && reg <= ATA_REG_LBA5)
    {
        ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN);
    }

    return result;
}
void ide_read_buffer(uint8_t channel, uint8_t reg, uint32_t * buffer, uint32_t quads){
    if (reg >= ATA_REG_SECTORCOUNT1 && reg <= ATA_REG_LBA5)
    {
        ide_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
    }

    if (reg <= ATA_REG_STATUS)
    {
        for (uint32_t i = 0; i < quads; i++)
        {
            *buffer = inportl(channels[channel].base + reg);
            buffer++;
        }
        
    }
    else if (reg <= ATA_REG_LBA5)
    {
        for (uint32_t i = 0; i < quads; i++)
        {
            *buffer = inportl(channels[channel].base + reg - 0x06);
            buffer++;
        }
    }
    else if (reg <= ATA_REG_DEVADDRESS)
    {
        for (uint32_t i = 0; i < quads; i++)
        {
            *buffer = inportl(channels[channel].ctrl + reg - 0x0C);
            buffer++;
        }
    }
    else if (reg < 0x16)
    {
        for (uint32_t i = 0; i < quads; i++)
        {
            *buffer = inportl(channels[channel].bmide + reg - 0x0E);
            buffer++;
        }
    }

    if (reg >= ATA_REG_SECTORCOUNT1 && reg <= ATA_REG_LBA5)
    {
        ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN);
    }
}
void io_wait(uint8_t channel){
    ide_read(channel,ATA_REG_ALTSTATUS);
    ide_read(channel,ATA_REG_ALTSTATUS);
    ide_read(channel,ATA_REG_ALTSTATUS);
    ide_read(channel,ATA_REG_ALTSTATUS);
}
uint8_t ide_polling(uint8_t channel){
    io_wait(channel);

    uint8_t status;
    while (BITREAD(status = ide_read(channel,ATA_REG_STATUS),ATA_SR_BSY)){}
    
    if(BITREAD(status,ATA_SR_ERR))
        return 2;
    if(BITREAD(status,ATA_SR_DF))
        return 1;
    if(BITREAD(status,ATA_SR_DRQ)==0)
        return 3;

    return 0;
    
}
void enable_irq(uint8_t channel){
    if(channel==ATA_PRIMARY){
        ide_irq0_invoked = 0;
    }
    else
    {
        ide_irq1_invoked = 0;
    }
    channels[channel].nIEN = 0;
    ide_write(channel,ATA_REG_CONTROL,0b00000000);
}
void disable_irq(uint8_t channel){
    channels[channel].nIEN = 0;
    ide_write(channel,ATA_REG_CONTROL,0b00000010);
}
void wait_if_busy(uint8_t channel){
    while (BITREAD(ide_read(channel,ATA_REG_STATUS),ATA_SR_BSY)){ } 
}
void ide_wait_irq(uint8_t channel){
    if(channel == ATA_PRIMARY){
        while (!ide_irq0_invoked){ }
        ide_irq0_invoked=0;    
    }
    else{
        while (!ide_irq1_invoked){ }
        ide_irq1_invoked=0;    
    }
    
}
void irq_primary_handler(register_t * reg){
    ide_irq0_invoked =1;
    log_trace("IDE: Primary Channel IRQ Arrived");
}
void irq_secondary_handler(register_t * reg){
    ide_irq1_invoked =1;
    log_trace("IDE: Secondary Channel IRQ Arrived");
}
uint8_t ide_ata_access(uint8_t direction,uint8_t drive, uint32_t lba, uint8_t numsectors,ptr_t buffer){
    uint8_t lba_mode/*0:CHS;1:LBA28;2LBA48*/, dma/*0:NO_DMA;1:DMA*/, cmd;
    uint8_t lba_io[6];
    uint32_t channel = devices[drive].channel;
    uint32_t slave = devices[drive].drive;
    uint32_t bus = channels[channel].base;
    uint32_t sector_size = 512;
    uint16_t cyl,i;
    uint8_t head,sect,err;

    //disable IRQ
    disable_irq(channel);


    if(lba >= 0x10000000)//>28Line
    {
        //LBA84
        lba_mode = 2;
        lba_io[0] = (lba & 0x000000FF) >> 0;
        lba_io[1] = (lba & 0x0000FF00) >> 8;
        lba_io[2] = (lba & 0x00FF0000) >> 16;
        lba_io[3] = (lba & 0xFF000000) >> 24;
        lba_io[4] = 0;
        lba_io[5] = 0;
        head = 0;
    }else if(devices[drive].capablities & 0x200)//is Support LBA
    {
        //LBA28
        lba_mode = 1;
        lba_io[0] = (lba & 0x000000FF) >> 0;
        lba_io[1] = (lba & 0x0000FF00) >> 8;
        lba_io[2] = (lba & 0x00FF0000) >> 16;
        lba_io[3] = 0;
        lba_io[4] = 0;
        lba_io[5] = 0;
        head = (lba & 0x0F000000) >> 24;
    }else
    {
        // CHS:
      lba_mode  = 0;
      sect      = (lba % 63) + 1;
      cyl       = (lba + 1  - sect) / (16 * 63);
      lba_io[0] = sect;
      lba_io[1] = (cyl >> 0) & 0xFF;
      lba_io[2] = (cyl >> 8) & 0xFF;
      lba_io[3] = 0;
      lba_io[4] = 0;
      lba_io[5] = 0;
      head      = (lba + 1  - sect) % (16 * 63) / (63); // Head number is written to HDDEVSEL lower 4-bits.
    }

    dma = is_dma_support; 
    wait_if_busy(channel);

    /**
     * Select DRIVE REGISTER BITS
     * bits 0:3 = Head number for CHS mode
     * bit 4 => 0 = Master, 1= Slave
     * bit 5 should be set to 1
     * bit 6 lba => 0=CHS, 1=LBA
     * bit 7 should be set to 1;
     */
    if(lba_mode == 0){
        ide_write(channel,ATA_REG_DRIVESELECT,0xA0 | (slave << 4) | head);
    }else{
        ide_write(channel,ATA_REG_DRIVESELECT,0xE0 | (slave << 4) | head);
    }

    if(lba_mode == 2){
        //LBA48
        ide_write(channel, ATA_REG_SECTORCOUNT1,0);
        ide_write(channel, ATA_REG_LBA3,   lba_io[3]);
        ide_write(channel, ATA_REG_LBA4,   lba_io[4]);
        ide_write(channel, ATA_REG_LBA5,   lba_io[5]);
    }

    ide_write(channel, ATA_REG_SECTORCOUNT0, numsectors);
    ide_write(channel, ATA_REG_LBA0,   lba_io[0]);
    ide_write(channel, ATA_REG_LBA1,   lba_io[1]);
    ide_write(channel, ATA_REG_LBA2,   lba_io[2]);

    if (lba_mode == 0 && dma == 0 && direction == 0) cmd = ATA_CMD_READ_PIO;
    if (lba_mode == 1 && dma == 0 && direction == 0) cmd = ATA_CMD_READ_PIO;   
    if (lba_mode == 2 && dma == 0 && direction == 0) cmd = ATA_CMD_READ_PIO_EXT;   
    if (lba_mode == 0 && dma == 1 && direction == 0) cmd = ATA_CMD_READ_DMA;
    if (lba_mode == 1 && dma == 1 && direction == 0) cmd = ATA_CMD_READ_DMA;
    if (lba_mode == 2 && dma == 1 && direction == 0) cmd = ATA_CMD_READ_DMA_EXT;
    if (lba_mode == 0 && dma == 0 && direction == 1) cmd = ATA_CMD_WRITE_PIO;
    if (lba_mode == 1 && dma == 0 && direction == 1) cmd = ATA_CMD_WRITE_PIO;
    if (lba_mode == 2 && dma == 0 && direction == 1) cmd = ATA_CMD_WRITE_PIO_EXT;
    if (lba_mode == 0 && dma == 1 && direction == 1) cmd = ATA_CMD_WRITE_DMA;
    if (lba_mode == 1 && dma == 1 && direction == 1) cmd = ATA_CMD_WRITE_DMA;
    if (lba_mode == 2 && dma == 1 && direction == 1) cmd = ATA_CMD_WRITE_DMA_EXT;
    ide_write(channel, ATA_REG_COMMAND, cmd);               // Send the Command.

    if(dma){

        /*
            command
            bit
            0   => START/Stop Bit 0=Normal, 1=DMA MODE
            1:6 => Must Be 0;
            7   => Read/Write 0=Write , 1=Read
        */

       /*
            Status
            0   =>   1 = DMA is Already Active   0=Last PRD has been used up.
            1   =>   1 = DMA Transfer Failed     0=DMA Successfully Done
            2   =>   1 =                         0=IRQ RECIEVED AND OS Handle IT (must restet this bin after every irq)
            3,4 =>   unused
            5,6 =>   Initial bits set by bios if the master or slave drive are respictively
            7   =>   obsolete for alod computers
       */
        
        channels[channel].prdt.trans_size = sector_size;
        channels[channel].prdt.msb_mark = 0x8000;

        if(direction == ATA_READ){
            // DMA_READ
            enable_irq(channel);
            outportl(channels[channel].bmide + DMA_PRDT,channels[channel].prdt_physical);
            outportb(channels[channel].bmide + DMA_COMMAND,0x8 | 0x1);
            //ide_polling(channel);
            waitirq_read:
            ide_wait_irq(channel);
            uint8_t dma_device_status = inportb(channels[channel].bmide + DMA_STATUS);
            if(BITREAD(dma_device_status,2) == 0){ //if irq arraived and bit num 2 not set so this irq not for us wait again
                goto waitirq_read;
            }
            outportb(channels[channel].bmide + DMA_STATUS,0); // Reset IRQ;
            outportb(channels[channel].bmide + DMA_COMMAND,0); // Reset and stop DMA device;
            if(BITREAD(dma_device_status,1) == 1){
                disable_irq(channel);
                return 3;//DMA Failed To READ
            }

            uint8_t device_status = ide_read(channel,ATA_REG_STATUS);
            if(BITREAD(device_status,ATA_SR_ERR)){
                disable_irq(channel);
                return 4; // device read error
            }


            //copy from prdt to buffer
            memcpy(buffer,channels[channel].dma_buffer,sector_size);
            disable_irq(channel);
        }else{
            //copy from buffer to prdt
            memcpy(channels[channel].dma_buffer,buffer,sector_size);
            enable_irq(channel);
            // DMA_Write
            outportl(channels[channel].bmide + DMA_PRDT,channels[channel].prdt_physical);
            outportb(channels[channel].bmide + DMA_COMMAND,0x1);
            //ide_polling(channel);
            waitirq_write:
            ide_wait_irq(channel);
            uint8_t dma_device_status = inportb(channels[channel].bmide + DMA_STATUS);
            if(BITREAD(dma_device_status,2) == 0){ //if irq arraived and bit num 2 not set so this irq not for us wait again
                goto waitirq_write;
            }
            outportb(channels[channel].bmide + DMA_STATUS,0); // Reset IRQ;
            outportb(channels[channel].bmide + DMA_COMMAND,0); // Reset and stop DMA device;
            if(BITREAD(dma_device_status,1) == 1){
                disable_irq(channel);
                return 3;//DMA Failed To write
            }

            uint8_t device_status = ide_read(channel,ATA_REG_STATUS);
            if(BITREAD(device_status,ATA_SR_ERR) || BITREAD(device_status,ATA_SR_DF)){
                disable_irq(channel);
                return 4; // device write error
            }
            disable_irq(channel);
        }
    }
    else{
        if(direction == ATA_READ){
            //PIO_READ

            uint16_t sector_size_in_words = sector_size/2;
            for (uint8_t sector = 0; sector < numsectors; sector++)
            {
                if((err = ide_polling(channel))){
                    return err;
                }

                uint16_t * sector_buffer = (uint16_t *)buffer;
                uint32_t sector_offset_words = sector * sector_size_in_words;

                for (uint32_t seek = 0; seek < sector_size_in_words; seek++)
                {
                    uint32_t offset = sector_offset_words + seek;
                    *sector_buffer = inports(bus);
                    sector_buffer++;
                }
                
            }
            
        }
        else
        {
            //PIO WRITE
            uint16_t sector_size_in_words = sector_size/2;
            for (uint8_t sector = 0; sector < numsectors; sector++)
            {
                if((err = ide_polling(channel))){
                    return err;
                }

                uint16_t * sector_buffer = (uint16_t *)buffer;
                uint32_t sector_offset_words = sector * sector_size_in_words;

                for (uint32_t seek = 0; seek < sector_size_in_words; seek++)
                {
                    uint32_t offset = sector_offset_words + seek;
                    outports(bus,*sector_buffer);
                    sector_buffer++;
                }

                ide_write(channel,ATA_REG_COMMAND,(char[]){ATA_CMD_CACHE_FLUSH,ATA_CMD_CACHE_FLUSH,ATA_CMD_CACHE_FLUSH_EXT}[lba_mode]);
                ide_polling(channel);
            }
        }
    }

    return 0;
}
uint8_t ide_atapi_read(uint8_t drive,uint32_t lba, uint8_t numsectors,ptr_t buffer){
    uint32_t channel  = devices[drive].channel;
    uint32_t slave    = devices[drive].drive;
    uint32_t bus      = channels[channel].base;
    uint32_t sector_size_words = 1024; // Sector Size. ATAPI drives have a sector size of 2048 bytes.
    uint8_t  err;

    enable_irq(channel);

    atapi_packet[0] = ATAPI_CMD_READ;
    atapi_packet[1] = 0;
    atapi_packet[2] = (lba >> 24) & 0xFF;
    atapi_packet[3] = (lba >> 16) & 0xFF;
    atapi_packet[4] = (lba >> 8) & 0xFF;
    atapi_packet[5] = (lba >> 0) & 0xFF;
    atapi_packet[6] = 0 ;
    atapi_packet[7] = 0 ;
    atapi_packet[8] = 0 ;
    atapi_packet[9] = numsectors ;
    atapi_packet[10] = 0 ;
    atapi_packet[11] = 0 ;

    ide_write(channel,ATA_REG_DRIVESELECT,slave<<4);

    io_wait(channel);

    if(is_dma_support){
        ide_write(channel,ATA_REG_FEATURES,1); //DMA Mode
    }else{
        ide_write(channel,ATA_REG_FEATURES,0); //POI Mode
    }

    ide_write(channel,ATA_REG_LBA1,(sector_size_words * 2) & 0xFF);
    ide_write(channel,ATA_REG_LBA2,(sector_size_words * 2) >> 8);

    ide_write(channel,ATA_REG_COMMAND,ATA_CMD_PACKET);

    if((err=ide_polling(channel)))return err;
    uint16_t * packet = (uint16_t *)atapi_packet;
    for (uint8_t i = 0; i < 6; i++)
    {
        outports(bus,* packet);
        packet++;
    }

    if(is_dma_support){
        channels[channel].prdt.trans_size = 0;
        channels[channel].prdt.msb_mark = 0x8000;
        outportl(channels[channel].bmide + DMA_PRDT,channels[channel].prdt_physical);
        outportb(channels[channel].bmide + DMA_COMMAND,0x8 | 0x1);
        // uint8_t err = ide_polling(channel);
        waitirq:
        ide_wait_irq(channel);
        uint8_t dma_device_status = inportb(channels[channel].bmide + DMA_STATUS);
        if(BITREAD(dma_device_status,2) == 0){ //if irq arraived and bit num 2 not set so this irq not for us wait again
            goto waitirq;
        }
        outportb(channels[channel].bmide + DMA_STATUS,0); // Reset IRQ;
        outportb(channels[channel].bmide + DMA_COMMAND,0); // Reset and stop device;
        if(BITREAD(dma_device_status,1) == 1){
            return 3;//DMA Failed To READ
        }

        uint8_t device_status = ide_read(channel,ATA_REG_STATUS);
        if(BITREAD(device_status,ATA_SR_ERR)){
            return 4; // device read error
        }

        //copy from prdt to buffer
        memcpy(buffer,channels[channel].dma_buffer,sector_size_words *2);
    }
    else
    {
        for (uint8_t sector = 0; sector < numsectors; sector++)
        {
            if((err = ide_polling(channel))){
                return err;
            }

            uint16_t * sector_buffer = (uint16_t *)buffer;
            uint32_t sector_offset_words = sector * sector_size_words;

            for (uint32_t seek = 0; seek < sector_size_words; seek++)
            {
                uint32_t offset = sector_offset_words + seek;
                *sector_buffer = inports(bus);
                sector_buffer++;
            }
            
        }
        ide_wait_irq(channel);
    }
    

    

    while (BITREAD(ide_read(channel, ATA_REG_STATUS),ATA_SR_BSY) || BITREAD(ide_read(channel, ATA_REG_STATUS),ATA_SR_DRQ)){ }
    return 0;
}
uint8_t ide_read_sectors(uint8_t drive,uint8_t numsectors,uint32_t lba,ptr_t buffer){
    if(drive>3 || devices[drive].present == 0){
        return 0x9;
    }

    if((lba+numsectors)>devices[drive].size && devices[drive].type == IDE_ATA){
        return 0x8;
    }

    uint8_t err;
    if(devices[drive].type == IDE_ATA){
        err = ide_ata_access(ATA_READ,drive,lba,numsectors,buffer);
    }
    else if(devices[drive].type == IDE_ATAPI)
    {
        ptr_t sector_buffer = buffer;
        for (uint8_t i = 0; i < numsectors; i++)
        {
            sector_buffer += i*2048;
            err = ide_atapi_read(drive,lba+i,1,sector_buffer);
        }
    }
    else
    {
        //Unknown device;
        return 0x07;
    }
    
    return err;

}
uint8_t ide_write_sectors(uint8_t drive,uint8_t numsectors,uint32_t lba,ptr_t buffer){
   if(drive>3 || devices[drive].present == 0){
        return 0x9;
    }

    if((lba+numsectors)>devices[drive].size && devices[drive].type == IDE_ATA){
        return 0x8;
    }

    uint8_t err;
    if(devices[drive].type == IDE_ATA){
        err = ide_ata_access(ATA_WRITE,drive,lba,numsectors,buffer);
    }
    else if(devices[drive].type == IDE_ATAPI)
    {
      err = 4;
    }
    else
    {
        //Unknown device;
        return 0x07;
    }
    
    return err; 
}
uint32_t ide_vfs_read(FsNode * node,uint32_t offset, uint32_t size,uint8_t *buffer){
    struct ide_device device = devices[node->inode];
    uint32_t sector_size = device.type == IDE_ATA?512:2048;
    uint32_t lba = offset/sector_size;
    uint32_t lba_offset = offset%sector_size;

    uint32_t lba_end = (offset + size -1) / sector_size;
    uint32_t lba_end_offset = (offset + size -1) % sector_size;

    uint8_t * buf_curr = buffer;
    uint32_t counter = lba;
    uint32_t read_size ;
    uint32_t off,total=0;
    ptr_t sector_buffer = kmalloc(2048);
    while (counter  <= lba_end)
    {
        off = 0;
        read_size = sector_size;
        ide_read_sectors(node->inode,1,counter,sector_buffer);
        if(counter == lba){
            off=lba_offset;
            read_size = sector_size - off;
        }
        if (counter == lba_end)
        {
            read_size = lba_end_offset;
        }

        memcpy(buf_curr,sector_buffer+off,read_size);
        buf_curr += read_size;
        total = total+read_size;
        counter ++;
    }

    kfree(sector_buffer);
    return total;
    

}
uint32_t ide_vfs_write(FsNode * node,uint32_t offset, uint32_t size,uint8_t *buffer){
    return 0;
}
uint8_t ide_vfs_mount_device(uint8_t drive){
    if(drive>3 || !devices[drive].present)return 1;//Drive Not Found

    FsNode *vfs_node = (FsNode *)kmalloc(sizeof(FsNode));
    vfs_node->inode = drive;
    strcpy(vfs_node->name,(const char * []){"hda","hdb","hdc","hdd"}[drive]);
    vfs_node->flags |= FS_BLOCKDEVICE;
    vfs_node->read = ide_vfs_read;
    vfs_node->write = ide_vfs_write;
    VfsMount((char * []){"/dev/hda","/dev/hdb","/dev/hdc","/dev/hdd"}[drive],vfs_node);
    return 0;
}
void ide_install(pci_config_t * ide_device)
{
    pci_device_config_t * _ide_device = (pci_device_config_t *)ide_device->config;

    uint32_t BAR0=_ide_device->BAR0,
             BAR1=_ide_device->BAR1,
             BAR2=_ide_device->BAR2, 
             BAR3=_ide_device->BAR3, 
             BAR4=_ide_device->BAR4;

    uint8_t PROGIF=_ide_device->common.prog_if;

    if(BITREAD(PROGIF,0)==1)
    {
        //Primary Channel Native mode
        if(BITREAD(PROGIF,1)==1){
            // can i convert native mode to compitibilty mode
            //TODO Switch To Compitibilty mode
        }
        else{
            log_error("PCI IDE Primary Channel In Native Mode and cannot switch to compitiblty mode");
        }

    }

    if(BITREAD(PROGIF,2)==1)
    {
        //Secondary Channel Native mode
        if(BITREAD(PROGIF,3)==1)
        {
            // can i convert native mode to compitibilty mode
            //TODO Switch To Compitibilty mode
        }
        else
        {
            log_error("PCI IDE Primary Channel In Native Mode and cannot switch to compitiblty mode");
        }

    }

    if(BITREAD(PROGIF,7)==0){
        log_warning("PCI IDE Dosn't Support DMA Mode And Will Use PIO Mode");
    }



    //Primary channel in compatibilty mode port (0x1F0->0x1F7,0x3F6,IRQ14)
    channels[ATA_PRIMARY].base = (BAR0 & 0xFFFFFFFC) + 0x1F0 * (!BAR0);
    channels[ATA_PRIMARY].ctrl = (BAR1 & 0xFFFFFFFC) + 0x3F6 * (!BAR1);
    channels[ATA_PRIMARY].bmide = BAR4 & 0xFFFFFFFC ;

    channels[ATA_SECONDARY].base = (BAR0 & 0xFFFFFFFC) + 0x170 * (!BAR0);
    channels[ATA_SECONDARY].ctrl = (BAR1 & 0xFFFFFFFC) + 0x376 * (!BAR1);
    channels[ATA_SECONDARY].bmide = (BAR4 & 0xFFFFFFFC) + 8 ;

    // 2- Disable IRQs:
    disable_irq(ATA_PRIMARY);
    disable_irq(ATA_SECONDARY);
    
    int32_t channel=0, drive=0, k=0, count = 0;
    for (channel = 0; channel < 2; channel++)
    {
        for (drive = 0; drive < 2; drive ++,count++)
        {
            uint8_t err=0, type=IDE_ATA, status=0;
            devices[count].present = 0;

            ide_write(channel,ATA_REG_DRIVESELECT,0xA0 | (drive << 4));
            io_wait(channel);

            ide_write(channel,ATA_REG_COMMAND,ATA_CMD_IDENTIFY);
            io_wait(channel);

            if(ide_read(channel,ATA_REG_STATUS) == 0) continue ;// No Device

            while (1)
            {
                status = ide_read(channel,ATA_REG_STATUS);
                if(BITREAD(status,ATA_SR_ERR)){
                    err = 1;
                    break ;
                }
                if(!BITREAD(status,ATA_SR_BSY) && BITREAD(status,ATA_SR_DRQ))break;
            }

            if(err != 0){
                uint8_t cl = ide_read(channel,ATA_REG_LBA1);
                uint8_t ch = ide_read(channel,ATA_REG_LBA2);

                if(cl == 0x14 && ch == 0xEB){
                    type = IDE_ATAPI;
                }else if(cl == 0x69 && ch==0x96){
                    type = IDE_ATAPI;
                }
                else{
                    continue;
                }

                ide_write(channel,ATA_REG_COMMAND,ATA_CMD_IDENTIFY_PACKET);
                io_wait(channel);
            }

            ide_read_buffer(channel,ATA_REG_DATA,(uint32_t *)ide_buffer,128);

            devices[count].present = 1;
            devices[count].type = type;
            devices[count].channel = channel;
            devices[count].drive = drive;
            devices[count].signature = *((uint16_t *)(ide_buffer + ATA_IDENT_DEVICETYPE));
            devices[count].capablities = *((uint16_t *)(ide_buffer + ATA_IDENT_CAPABILITIES));
            devices[count].command_sets = *((uint32_t *)(ide_buffer + ATA_IDENT_COMMANDSETS));

            if(devices[count].command_sets & (1 << 26)){
                //Device Uses 48-bit Addressing
                devices[count].size = *((unsigned int *)(ide_buffer + ATA_IDENT_MAX_LBA_EXT));
            }
            else
            {
                //Device Uses 28-bit Addressing
                devices[count].size = *((unsigned int *)(ide_buffer + ATA_IDENT_MAX_LBA));
            }

            for (k = 0; k < 40; k+= 2)
            {
                devices[count].model[k] = ide_buffer[ATA_IDENT_MODEL +k +1];
                devices[count].model[k + 1] = ide_buffer[ATA_IDENT_MODEL + k];
            }
            devices[count].model[40] = 0;
            ide_vfs_mount_device(count);
        } 
    }

    if(BAR4 != 0 && BITREAD(BAR4,0)){
        // Setup IDE Busmastering
        allocate_page(kernel_directory,VIRTUAL_DMA_ADDRESS,0,1);
        memset((void *)VIRTUAL_DMA_ADDRESS,0,PAGE_SIZE);
        for (uint8_t channel = 0; channel < 2; channel++)
        {
            channels[channel].dma_buffer = (ptr_t)VIRTUAL_DMA_ADDRESS + (channel * (PAGE_SIZE / 2));
            channels[channel].prdt.ph_addr = virtual2physical((v_addr_t)channels[channel].dma_buffer);
            channels[channel].prdt_physical = virtual2physical((v_addr_t)&channels[channel].prdt);
            channels[channel].prdt.trans_size = 0;
            channels[channel].prdt.msb_mark = 0x8000;
            outportb(channels[channel].bmide + DMA_COMMAND,0);
        }
        

        //Enable IDE Bus Mastering
        uint32_t dev_command = _ide_device->common.command;
        dev_command |= (1<<2);
        pci_writew(ide_device->bus,ide_device->device,ide_device->function,PCI_COMMAND,dev_command);
        is_dma_support = 1;
    }
    

    for (uint8_t i = 0; i < 4; i++)
    {
        if(devices[i].present == 1){
            log_information("Found %s Drive %dKB - %s",
                (const char *[]){"ATA","ATAPI"}[devices[i].type],
                devices[i].size / 2,
                devices[i].model
            );
        }
    }
    
    register_interrupt_handler(IRQ_BASE+IRQ14_HARD_DISK,irq_primary_handler);
    register_interrupt_handler(IRQ_BASE+IRQ15_RESERVED,irq_secondary_handler);
}
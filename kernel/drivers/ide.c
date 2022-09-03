#include <kernel/drivers/ide.h>
#include <kernel/drivers/pci.h>
#include <kernel/mem/vmm.h>
#include <kernel/mem/kheap.h>
#include <kernel/filesystems/vfs.h>
#include <kernel/system.h>
#include <kernel/bits.h>
#include <logger.h>
#include <string.h>
#include <stddef.h>

uint8_t ide_buffer[0x800] = {0};
static volatile uint8_t ide_irq_invoked = 0;
static int8_t atapi_packet[12] = {0xA8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
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
void ide_install(uint32_t BAR0, uint32_t BAR1, uint32_t BAR2, uint32_t BAR3, uint32_t BAR4)
{
    channels[ATA_PRIMARY].base = (BAR0 & 0xFFFFFFFC) + 0x1F0 * (!BAR0);
    channels[ATA_PRIMARY].ctrl = (BAR1 & 0xFFFFFFFC) + 0x3F6 * (!BAR1);
    channels[ATA_PRIMARY].bmide = BAR4 & 0xFFFFFFFC ;

    channels[ATA_SECONDARY].base = (BAR0 & 0xFFFFFFFC) + 0x170 * (!BAR0);
    channels[ATA_SECONDARY].ctrl = (BAR1 & 0xFFFFFFFC) + 0x376 * (!BAR1);
    channels[ATA_SECONDARY].bmide = (BAR4 & 0xFFFFFFFC) + 8 ;

    // 2- Disable IRQs:
    ide_write(ATA_PRIMARY  , ATA_REG_CONTROL, 2);
    ide_write(ATA_SECONDARY, ATA_REG_CONTROL, 2);
    
    int32_t channel=0, drive=0, k=0, count = 0;
    for (channel = 0; channel < 2; channel++)
    {
        for (drive = 0; drive < 2; drive ++)
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
            count ++;
        }
        
    }

    for (uint8_t i = 0; i < 4; i++)
        {
            if(devices[i].present == 1){
                log_information("Found %s Drive %dGB - %s",
                    (const char *[]){"ATA","ATAPI"}[devices[i].type],
                    devices[i].size / 1024 / 1024 / 2,
                    devices[i].model
                );
            }
        }
    
}
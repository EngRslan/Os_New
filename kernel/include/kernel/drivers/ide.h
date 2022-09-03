#ifndef IDE_H
#define IDE_H
#include <kernel/types.h>

#define IDE_PRIMARY_ADDRESS 0x1F0
#define IDE_SECONDARY_ADDRESS 0x170


#define ATA_DATA(x)         (x)
#define ATA_FEATURES(x)     (x+1)
#define ATA_SECTOR_COUNT(x) (x+2)
#define ATA_ADDRESS1(x)     (x+3)
#define ATA_ADDRESS2(x)     (x+4)
#define ATA_ADDRESS3(x)     (x+5)
#define ATA_DRIVE_SELECT(x) (x+6)
#define ATA_COMMAND(x)      (x+7)
#define ATA_DCR(x)          (x+0x206)

typedef struct prdt
{
    uint32_t buffer_phys;
    uint16_t transfer_size;
    uint16_t mark_end;
} prdt_t;

typedef struct ide_device
{
    uint16_t data_addr;
    uint16_t feature_addr;
    uint16_t sector_addr;
    uint16_t addr1;
    uint16_t addr2;
    uint16_t addr3;
    uint16_t drive;
    uint16_t command;
    uint32_t dcr_addr;

	uint32_t BMR_COMMAND;
	uint32_t BMR_prdt;
	uint32_t BMR_STATUS;


    int32_t slave;
    prdt_t * prdt;
    uint8_t * prdt_phys;
    uint8_t * mem_Buffer;
    uint8_t * mem_buffer_phys;

  char mountpoint[32];

} ide_device_t;

void ide_install();
#endif
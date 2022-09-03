#ifndef IDE_H
#define IDE_H
#include <kernel/types.h>


/*
    Status Register MASK
*/
    
// BUSY               BIT ORDER
#define ATA_SR_BSY      0x07
// Drive_READY
#define ATA_SR_DRDY     0x06
// DRIVE_FAULT
#define ATA_SR_DF       0x05
// DRIVE_SEEK_COMPLETE
#define ATA_SR_DSC      0x04
// DATA_REQUEST_READY
#define ATA_SR_DRQ      0x03
// CORRECTED_DATA
#define ATA_SR_CORR     0x02
// INDEX
#define ATA_SR_IDX      0x01
// ERROR
#define ATA_SR_ERR      0x00


/*
    Error Register MASK
*/
// BAD BLOCK
#define ATA_ER_BBK      0x08
// UNCORRECTED DATA
#define ATA_ERR_UNC     0x07
// MEDIA CHANGED
#define ATA_ERR_MC      0x06
// ID MARK NOT FOUND
#define ATA_ERR_IDNF    0x05
// MEDIA CHANGE REQUEST
#define ATA_ERR_MCR     0x04
// COMMAND ABORTED
#define ATA_ERR_ABRT    0x03
// TRACK 0 NOT FOUND
#define ATA_ERR_TK0NF   0x02
// NO ADDRESS MARK
#define ATA_ERR_AMNF    0x01


/*
    Command Register
*/
#define ATA_CMD_READ_PIO        0x20
#define ATA_CMD_READ_PIO_EXT    0x24
#define ATA_CMD_READ_DMA        0xC8
#define ATA_CMD_READ_DMA_EXT    0x25
#define ATA_CMD_WRITE_PIO       0x30
#define ATA_CMD_WRITE_PIO_EXT   0x34
#define ATA_CMD_WRITE_DMA       0xCA
#define ATA_CMD_WRITE_DMA_EXT   0x35
#define ATA_CMD_CACHE_FLUSH     0xE7
#define ATA_CMD_CACHE_FLUSH_EXT 0xEA
#define ATA_CMD_PACKET          0xA0
#define ATA_CMD_IDENTIFY_PACKET 0xA1
#define ATA_CMD_IDENTIFY        0xEC
#define ATAPI_CMD_READ          0xA8
#define ATAPI_CMD_EJECT         0x1B


/*
    Identification Space Offsets
*/
#define ATA_IDENT_DEVICETYPE   0
#define ATA_IDENT_CYLINDERS    2
#define ATA_IDENT_HEADS        6
#define ATA_IDENT_SECTORS      12
#define ATA_IDENT_SERIAL       20
#define ATA_IDENT_MODEL        54
#define ATA_IDENT_CAPABILITIES 98
#define ATA_IDENT_FIELDVALID   106
#define ATA_IDENT_MAX_LBA      120
#define ATA_IDENT_COMMANDSETS  164
#define ATA_IDENT_MAX_LBA_EXT  200
/*
    Channels
*/
#define ATA_PRIMARY     0x00
#define ATA_SECONDARY   0x01
/*
    Directions    
*/
#define ATA_READ        0x00
#define ATA_WRITE       0x01
/*
    Media Types
*/
#define IDE_ATA     0x00
#define IDE_ATAPI   0x01
/*
    Drivers Types
*/
#define ATA_MASTER  0x00
#define ATA_SLAVE   0x01

/*
    Registers Ports Offsets
*/
#define ATA_REG_DATA            0x00
#define ATA_REG_ERROR           0x01
#define ATA_REG_FEATURES        0x01
#define ATA_REG_SECTORCOUNT0    0x02
#define ATA_REG_LBA0            0x03
#define ATA_REG_LBA1            0x04
#define ATA_REG_LBA2            0x05
#define ATA_REG_DRIVESELECT     0x06
#define ATA_REG_COMMAND         0x07
#define ATA_REG_STATUS          0x07
#define ATA_REG_SECTORCOUNT1    0x08
#define ATA_REG_LBA3            0x09
#define ATA_REG_LBA4            0x0A
#define ATA_REG_LBA5            0x0B
#define ATA_REG_CONTROL         0x0C
#define ATA_REG_ALTSTATUS       0x0C
#define ATA_REG_DEVADDRESS      0x0D

/**
 * @brief IDE Channel Configuration
 * 
 */
struct ide_channel{
    uint16_t base;  // I/O BASE PORT
    uint16_t ctrl;  // CONTROL BASE PORT
    uint16_t bmide; // BUS MASTER IDE
    uint8_t  nIEN;  // No Onterrupt
} channels[2];

struct ide_device
{
    uint8_t  present;        // 0 = Empty Slot, 1= Exists Device
    uint8_t  channel;        // 0 = Primary, 1=Secondary
    uint8_t  drive;          // 0 = Master, 1=Slave
    uint16_t type;           // 0 = ATA, 1=ATAPI
    uint16_t signature;      // Drive Signature
    uint16_t capablities;    // Features
    uint32_t command_sets;   // Supported Command Sets
    uint32_t size;           // Size in Sectors
    uint8_t  model[41];      // Model string
} devices[4];

/**
 * @brief Initial IDE CONTROLLER
 * 
 * @param BAR0 PCI BAR0 FROM PCI Config Space
 * @param BAR1 PCI BAR1 FROM PCI Config Space
 * @param BAR2 PCI BAR2 FROM PCI Config Space
 * @param BAR3 PCI BAR3 FROM PCI Config Space
 * @param BAR4 PCI BAR4 FROM PCI Config Space
 */

void ide_install(uint32_t BAR0,uint32_t BAR1,uint32_t BAR2,uint32_t BAR3,uint32_t BAR4);
/**
 * @brief Read Data From ATA/ATAPI Devices
 * 
 * @param drive Drive Number 0 = Primary Master, 1 = Primary Slave, 2 = Secondary Master, 3 = Secondary Slave
 * @param numsectors Number of total sectors to read
 * @param lba // Start LBA Address to read from
 * @param buffer //Memory Buffer To Read Into;
 * @return uint8_t 
 */
uint8_t ide_read_sectors(uint8_t drive,uint8_t numsectors,uint32_t lba,ptr_t buffer);
uint8_t ide_write_sectors(uint8_t drive,uint8_t numsectors,uint32_t lba,ptr_t buffer);
#endif
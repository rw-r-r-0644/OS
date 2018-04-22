#pragma once

#include <stdint.h>

/* ATA Status */
#define ATA_SR_BSY     0x80    // Busy
#define ATA_SR_DRDY    0x40    // Drive ready
#define ATA_SR_DF      0x20    // Drive write fault
#define ATA_SR_DSC     0x10    // Drive seek complete
#define ATA_SR_DRQ     0x08    // Data request ready
#define ATA_SR_CORR    0x04    // Corrected data
#define ATA_SR_IDX     0x02    // Inlex
#define ATA_SR_ERR     0x01    // Error

/* ATA Errors */
#define ATA_ER_BBK      0x80    // Bad sector
#define ATA_ER_UNC      0x40    // Uncorrectable data
#define ATA_ER_MC       0x20    // No media
#define ATA_ER_IDNF     0x10    // ID mark not found
#define ATA_ER_MCR      0x08    // No media
#define ATA_ER_ABRT     0x04    // Command aborted
#define ATA_ER_TK0NF    0x02    // Track 0 not found
#define ATA_ER_AMNF     0x01    // No address mark

/* ATA Commands */
#define ATA_CMD_READ_PIO          0x20
#define ATA_CMD_READ_PIO_EXT      0x24
#define ATA_CMD_READ_DMA          0xC8
#define ATA_CMD_READ_DMA_EXT      0x25
#define ATA_CMD_WRITE_PIO         0x30
#define ATA_CMD_WRITE_PIO_EXT     0x34
#define ATA_CMD_WRITE_DMA         0xCA
#define ATA_CMD_WRITE_DMA_EXT     0x35
#define ATA_CMD_CACHE_FLUSH       0xE7
#define ATA_CMD_CACHE_FLUSH_EXT   0xEA
#define ATA_CMD_PACKET            0xA0
#define ATA_CMD_IDENTIFY_PACKET   0xA1
#define ATA_CMD_IDENTIFY          0xEC

/* ATAPI Commands */
#define      ATAPI_CMD_READ       0xA8
#define      ATAPI_CMD_EJECT      0x1B

/* ATA Identify */
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

/* IDE Interface type */
#define IDE_ATA        0x00
#define IDE_ATAPI      0x01
 
#define ATA_MASTER     0x00
#define ATA_SLAVE      0x01

/* ATA Registers */
#define ATA_REG_DATA       0x00
#define ATA_REG_ERROR      0x01
#define ATA_REG_FEATURES   0x01
#define ATA_REG_SECCOUNT0  0x02
#define ATA_REG_LBA0       0x03
#define ATA_REG_LBA1       0x04
#define ATA_REG_LBA2       0x05
#define ATA_REG_HDDEVSEL   0x06
#define ATA_REG_COMMAND    0x07
#define ATA_REG_STATUS     0x07
#define ATA_REG_SECCOUNT1  0x08
#define ATA_REG_LBA3       0x09
#define ATA_REG_LBA4       0x0A
#define ATA_REG_LBA5       0x0B
#define ATA_REG_CONTROL    0x0C
#define ATA_REG_ALTSTATUS  0x0C
#define ATA_REG_DEVADDRESS 0x0D

/* ATA Channels */
#define      ATA_PRIMARY      0x00
#define      ATA_SECONDARY    0x01
 
/* ATA Directions */
#define      ATA_READ      0x00
#define      ATA_WRITE     0x01

 typedef struct {
   u16 base;  // I/O Base.
   u16 ctrl;  // Control Base
   u16 bmide; // Bus Master IDE
   u8  nIEN;  // nIEN (No Interrupt);
} IDE_channel_registers;

typedef struct {
   u8  reserved;    // 0 (Empty) or 1 (This Drive really exists).
   u8  channel;     // 0 (Primary Channel) or 1 (Secondary Channel).
   u8  drive;       // 0 (Master Drive) or 1 (Slave Drive).
   u16 type;        // 0: ATA, 1:ATAPI.
   u16 signature;   // Drive Signature
   u16 capabilities;// Features.
   u32 command_sets;// Command Sets Supported.
   u32 size;        // Size in Sectors.
   char model[41];  // Model in string.
} IDE_device;

void ide_initialize(u32 BAR0, u32 BAR1, u32 BAR2, u32 BAR3, u32 BAR4);
u8 ide_ata_access(u8 direction, u8 drive, u32 lba, u8 numsects, u16 selector, u32 edi);
u8 ide_read_sectors(u8 drive, u8 numsects, u32 lba, u16 es, u32 edi);
u8 ide_write_sectors(u8 drive, u8 numsects, u32 lba, u16 es, u32 edi);
u8 ide_atapi_eject(u8 drive);

void init_ide_irq();
#include <drivers/ide.h>
#include <arch/x86/cpu.h>

#include <stdio.h>
#include <unistd.h>

IDE_channel_registers channels[2];
IDE_device ide_devices[4];

u8 ide_buf[2048] = {0};
static u8 ide_irq_invoked = 0;
static u8 atapi_packet[12] = {0xA8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void ide_write(u8 channel, u8 reg, u8 data)
{
	if (reg > 0x07 && reg < 0x0C)
		ide_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
	if (reg < 0x08)
		outb(channels[channel].base  + reg - 0x00, data);
	else if (reg < 0x0C)
		outb(channels[channel].base  + reg - 0x06, data);
	else if (reg < 0x0E)
		outb(channels[channel].ctrl  + reg - 0x0A, data);
	else if (reg < 0x16)
		outb(channels[channel].bmide + reg - 0x0E, data);
	if (reg > 0x07 && reg < 0x0C)
		ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN);
}

u8 ide_read(u8 channel, u8 reg)
{
	u8 result;
	if (reg > 0x07 && reg < 0x0C)
		ide_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
	if (reg < 0x08)
		result = inb(channels[channel].base + reg - 0x00);
	else if (reg < 0x0C)
		result = inb(channels[channel].base  + reg - 0x06);
	else if (reg < 0x0E)
		result = inb(channels[channel].ctrl  + reg - 0x0A);
	else if (reg < 0x16)
		result = inb(channels[channel].bmide + reg - 0x0E);
	if (reg > 0x07 && reg < 0x0C)
		ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN);
	return result;
}

void ide_read_buffer(u8 channel, u8 reg, void * buffer, u32 quads)
{
	/* WARNING: This code contains a serious bug. The inline assembly trashes ES and
	 *			  ESP for all of the code the compiler generates between the inline
	 *			  assembly blocks.
	 */
	if (reg > 0x07 && reg < 0x0C)
		ide_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
	asm("pushw %es; movw %ds, %ax; movw %ax, %es");
	if (reg < 0x08)
		insl(channels[channel].base  + reg - 0x00, buffer, quads);
	else if (reg < 0x0C)
		insl(channels[channel].base  + reg - 0x06, buffer, quads);
	else if (reg < 0x0E)
		insl(channels[channel].ctrl  + reg - 0x0A, buffer, quads);
	else if (reg < 0x16)
		insl(channels[channel].bmide + reg - 0x0E, buffer, quads);
	asm("popw %es;");
	if (reg > 0x07 && reg < 0x0C)
		ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN);
}

u8 ide_polling(u8 channel, u32 advanced_check)
{
	// Delay 400 nanosecond for BSY to be set
	for(int i = 0; i < 4; i++) // Reading the Alternate Status port wastes 100ns; loop four times
		ide_read(channel, ATA_REG_ALTSTATUS); 
  
	while (ide_read(channel, ATA_REG_STATUS) & ATA_SR_BSY); // Wait for BSY to be cleared
 
	if (advanced_check)
	{
		// Read Status Register
		u8 state = ide_read(channel, ATA_REG_STATUS);
		
		if (state & ATA_SR_ERR)			// Check for errors
			return 2;
		if (state & ATA_SR_DF)			// Check for device fault
			return 1;
		if ((state & ATA_SR_DRQ) == 0)	// Check DRQ
			return 3;
	}
	return 0;
}

const char *ide_errors[] = {
	0,					// 0
	"Device Fault",		// 1
	0,					// 2 (Needs checking)
	"Reads Nothing",	// 3
	"Write Protected",	// 4
	// Error 2 specific errors
	"No Address Mark Found",
	"No Media or Media Error",
	"Command Aborted",
	"No Media or Media Error",
	"ID mark not Found",
	"No Media or Media Error",
	"Uncorrectable Data Error",
	"Bad Sectors"
};

u8 ide_print_error(u32 drive, u8 err)
{
	if (err == 0)
		return err;
	
	if (err == 2)
	{
		u8 st = ide_read(ide_devices[drive].channel, ATA_REG_ERROR);
		if (st & ATA_ER_AMNF)	err = 5;
		if (st & ATA_ER_TK0NF)	err = 6;
		if (st & ATA_ER_ABRT)	err = 7;
		if (st & ATA_ER_MCR)	err = 8;
		if (st & ATA_ER_IDNF)	err = 9;
		if (st & ATA_ER_MC)		err = 10;
		if (st & ATA_ER_UNC)	err = 11;
		if (st & ATA_ER_BBK)	err = 12;
	}
	
	printf("[IDE]: %s %s %s: %s",	(ide_devices[drive].channel == 0) ? "Primary" : "Secondary",
									(ide_devices[drive].drive	== 0) ? "Master "  : "Slave ",
									(ide_devices[drive].model), 
									(ide_errors[err]));
	return err;
}

void ide_initialize(u32 BAR0, u32 BAR1, u32 BAR2, u32 BAR3, u32 BAR4)
{
	// Detect I/O Ports which interface IDE Controller
	channels[ATA_PRIMARY  ].base  = (BAR0 & 0xFFFFFFFC) + 0x1F0 * (!BAR0);
	channels[ATA_PRIMARY  ].ctrl  = (BAR1 & 0xFFFFFFFC) + 0x3F6 * (!BAR1);
	channels[ATA_SECONDARY].base  = (BAR2 & 0xFFFFFFFC) + 0x170 * (!BAR2);
	channels[ATA_SECONDARY].ctrl  = (BAR3 & 0xFFFFFFFC) + 0x376 * (!BAR3);
	channels[ATA_PRIMARY  ].bmide = (BAR4 & 0xFFFFFFFC) + 0; // Bus Master IDE
	channels[ATA_SECONDARY].bmide = (BAR4 & 0xFFFFFFFC) + 8; // Bus Master IDE
	
	// Disable IRQs
	ide_write(ATA_PRIMARY  , ATA_REG_CONTROL, 2);
	ide_write(ATA_SECONDARY, ATA_REG_CONTROL, 2);
	
	// Detect ATA-ATAPI Devices
	int count = 0;
	for(int i = 0; i < 2; i++)
	{
		for(int j = 0; j < 2; j++)
		{
 
			u8 err = 0, type = IDE_ATA, status;
			ide_devices[count].reserved = 0; // Assume there's no drive there
 
			// Select Drive
			ide_write(i, ATA_REG_HDDEVSEL, 0xA0 | (j << 4)); // Select Drive
			usleep(1000);	// Wait 1ms for drive select
 
			// Send ATA Identify Command
			ide_write(i, ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
			usleep(1000);	// Waits for 1 ms
			
			// Polling
			if (ide_read(i, ATA_REG_STATUS) == 0) // If Status = 0, No Device
				continue;
			
			for(;;)
			{
				status = ide_read(i, ATA_REG_STATUS);
				
				// If Err, Device is not ATA
				if ((status & ATA_SR_ERR)) {
					err = 1;
					break;
				}
				
				// Everything is right
				if (!(status & ATA_SR_BSY) && (status & ATA_SR_DRQ))
					break;
			}
 
			// Probe for ATAPI Devices
			if (err != 0)
			{
				u8 cl = ide_read(i, ATA_REG_LBA1);
				u8 ch = ide_read(i, ATA_REG_LBA2);
 
				if (cl == 0x14 && ch == 0xEB)
					type = IDE_ATAPI;
				else if (cl == 0x69 && ch == 0x96)
					type = IDE_ATAPI;
				else
					continue; // Unknown Type (may not be a device)
 
				ide_write(i, ATA_REG_COMMAND, ATA_CMD_IDENTIFY_PACKET);
				usleep(1000);
			}
 
			// Read Identification Space of the Device
			ide_read_buffer(i, ATA_REG_DATA, ide_buf, 128);
 
			// Read Device Parameters
			ide_devices[count].reserved		= 1;
			ide_devices[count].type			= type;
			ide_devices[count].channel		= i;
			ide_devices[count].drive		= j;
			ide_devices[count].signature	= *((u16 *)(ide_buf + ATA_IDENT_DEVICETYPE));
			ide_devices[count].capabilities	= *((u16 *)(ide_buf + ATA_IDENT_CAPABILITIES));
			ide_devices[count].command_sets	= *((u32 *)(ide_buf + ATA_IDENT_COMMANDSETS));
 
			// Get Size
			if (ide_devices[count].command_sets & (1 << 26))
				// Device uses 48-Bit Addressing
				ide_devices[count].size	= *((u32 *)(ide_buf + ATA_IDENT_MAX_LBA_EXT));
			else
				// Device uses CHS or 28-bit Addressing
				ide_devices[count].size	= *((u32 *)(ide_buf + ATA_IDENT_MAX_LBA));
 
			// String indicates model of device
			for(int k = 0; k < 40; k += 2) {
				ide_devices[count].model[k]		= ide_buf[ATA_IDENT_MODEL + k + 1];
				ide_devices[count].model[k + 1] = ide_buf[ATA_IDENT_MODEL + k];
			}
			ide_devices[count].model[40] = '\0'; // Terminate String

			count++;
		}
	}
 
	// Print detected drives
	for(int i = 0; i < 4; i++)
	{
		if (ide_devices[i].reserved == 1)
		{
			/*
			if (ide_devices[i].size > ((1024 ^ 2) * 2)) // Gb
				printf("%lu Gb", (ide_devices[i].size / 1024 / 1024 / 2));
			else if (ide_devices[i].size > ((1024) * 2)) // Mb
				printf("%lu Mb", (ide_devices[i].size / 1024 / 2));
			else // Kb
				printf("%lu Kb", (ide_devices[i].size / 2));
			*/
			printf("Found %s drive: %s\n", (ide_devices[i].type == 0) ? "ATA" : "ATAPI", ide_devices[i].model);
		}
	}
}

/* ATA/ATAPI Read/Write Modes:
 * ++++++++++++++++++++++++++++++++
 *  Addressing Modes:
 *  ================
 *	- LBA28 Mode.	  (+)
 *	- LBA48 Mode.	  (+)
 *	- CHS.				(+)
 *  Reading Modes:
 *  ================
 *	- PIO Modes (0 : 6)		 (+) // Slower than DMA, but not a problem.
 *	- Single Word DMA Modes (0, 1, 2).
 *	- Double Word DMA Modes (0, 1, 2).
 *	- Ultra DMA Modes (0 : 6).
 *  Polling Modes:
 *  ================
 *	- IRQs
 *	- Polling Status	(+) // Suitable for Singletasking	
*/

u8 ide_ata_access(u8 direction, u8 drive, u32 lba, u8 numsects, u16 selector, u32 edi)
{
	u8	lba_mode /* 0: CHS, 1:LBA28, 2: LBA48 */, dma /* 0: No DMA, 1: DMA */, cmd;
	u8	lba_io[6];
	u32	channel		= ide_devices[drive].channel; // Read the Channel
	u32	slavebit		= ide_devices[drive].drive; // Read the Drive [Master/Slave]
	u32	bus = channels[channel].base; // Bus Base, like 0x1F0 which is also data port
	u32	words		= 256; // Almost every ATA drive has a sector-size of 512-byte
	u16	cyl;
	u8	head, sect, err;
	
	ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN = (ide_irq_invoked = 0x0) + 0x02);
	
	// Select one from LBA28, LBA48 or CHS;
	if (lba >= 0x10000000) 	// Sure Drive should support LBA in this case, or you are giving a wrong LBA
	{
		// LBA48
		lba_mode  = 2;
		lba_io[0] = (lba & 0x000000FF) >> 0;
		lba_io[1] = (lba & 0x0000FF00) >> 8;
		lba_io[2] = (lba & 0x00FF0000) >> 16;
		lba_io[3] = (lba & 0xFF000000) >> 24;
		lba_io[4] = 0; // LBA28 is integer, so 32-bits are enough to access 2TB
		lba_io[5] = 0; // LBA28 is integer, so 32-bits are enough to access 2TB
		head		= 0; // Lower 4-bits of HDDEVSEL are not used here
	}
	else if (ide_devices[drive].capabilities & 0x200) // Drive supports LBA?
	{
		// LBA28
		lba_mode  = 1;
		lba_io[0] = (lba & 0x00000FF) >> 0;
		lba_io[1] = (lba & 0x000FF00) >> 8;
		lba_io[2] = (lba & 0x0FF0000) >> 16;
		lba_io[3] = 0; // These Registers are not used here
		lba_io[4] = 0; // These Registers are not used here
		lba_io[5] = 0; // These Registers are not used here
		head		= (lba & 0xF000000) >> 24;
	}
	else 
	{
		// CHS
		lba_mode  = 0;
		sect		= (lba % 63) + 1;
		cyl		 = (lba + 1  - sect) / (16 * 63);
		lba_io[0] = sect;
		lba_io[1] = (cyl >> 0) & 0xFF;
		lba_io[2] = (cyl >> 8) & 0xFF;
		lba_io[3] = 0;
		lba_io[4] = 0;
		lba_io[5] = 0;
		head		= (lba + 1  - sect) % (16 * 63) / (63); // Head number is written to HDDEVSEL lower 4-bits
	}
	
	// Check if drive supports DMA
	dma = 0; // We don't support DMA
	
	// Wait if the drive is busy
	while (ide_read(channel, ATA_REG_STATUS) & ATA_SR_BSY);
	
	// Select Drive from the controller
	if (lba_mode == 0)
		ide_write(channel, ATA_REG_HDDEVSEL, 0xA0 | (slavebit << 4) | head); // Drive & CHS
	else
		ide_write(channel, ATA_REG_HDDEVSEL, 0xE0 | (slavebit << 4) | head); // Drive & LBA
	
	// Write Parameters
	if (lba_mode == 2)
	{
		ide_write(channel, ATA_REG_SECCOUNT1,	0);
		ide_write(channel, ATA_REG_LBA3,	lba_io[3]);
		ide_write(channel, ATA_REG_LBA4,	lba_io[4]);
		ide_write(channel, ATA_REG_LBA5,	lba_io[5]);
	}
	ide_write(channel, ATA_REG_SECCOUNT0,	numsects);
	ide_write(channel, ATA_REG_LBA0,	lba_io[0]);
	ide_write(channel, ATA_REG_LBA1,	lba_io[1]);
	ide_write(channel, ATA_REG_LBA2,	lba_io[2]);
	
	// Select the command and send it;
	// Routine that is followed:
	// If ( DMA & LBA48)	DO_DMA_EXT;
	// If ( DMA & LBA28)	DO_DMA_LBA;
	// If ( DMA & LBA28)	DO_DMA_CHS;
	// If (!DMA & LBA48)	DO_PIO_EXT;
	// If (!DMA & LBA28)	DO_PIO_LBA;
	// If (!DMA & !LBA#)	DO_PIO_CHS;
	
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
	ide_write(channel, ATA_REG_COMMAND, cmd);					// Send the Command
	
	if (dma)
	{
		if (direction == 0) {} // DMA Read
		else {} // DMA Write
	}
	else
	{
		if (direction == 0)	// PIO Read
		{
			
			for (u16 i = 0; i < numsects; i++)
			{
				err = ide_polling(channel, 1);
				if (err)
					return err; // Polling, set error and exit if there is.
				asm("pushw %es");
				asm("mov %%ax, %%es" : : "a"(selector));
				asm("rep insw" : : "c"(words), "d"(bus), "D"(edi)); // Receive Data.
				asm("popw %es");
				edi += (words*2);
			}
		}
		else // PIO Write
		{ 
			for (u16 i = 0; i < numsects; i++)
			{
				ide_polling(channel, 0); // Polling
				asm("pushw %ds");
				asm("mov %%ax, %%ds"::"a"(selector));
				asm("rep outsw"::"c"(words), "d"(bus), "S"(edi)); // Send Data
				asm("popw %ds");
				edi += (words*2);
			}
			ide_write(channel, ATA_REG_COMMAND, (char []) {	ATA_CMD_CACHE_FLUSH, ATA_CMD_CACHE_FLUSH, ATA_CMD_CACHE_FLUSH_EXT}[lba_mode]);
			ide_polling(channel, 0); // Polling
		}
	}

	return 0; // Successfull
}

void ide_irq(registers_t *regs)
{
	ide_irq_invoked = 1;
}

void ide_wait_irq()
{
	sti(); // ....Needed?
	while (!ide_irq_invoked);
	ide_irq_invoked = 0;
}

void init_ide_irq()
{
	 // Install IDE irq
	register_interrupt_handler(IRQ14, ide_irq);
	register_interrupt_handler(IRQ15, ide_irq);
}

u8 ide_atapi_read(u8 drive, u32 lba, u8 numsects, u16 selector, u32 edi)
{
	u32	channel  = ide_devices[drive].channel;
	u32	slavebit = ide_devices[drive].drive;
	u32	bus		= channels[channel].base;
	u32	words	 = 1024; // Sector Size: ATAPI drives have a sector size of 2048 bytes
	u8  err;
	
	// Enable IRQs:
	ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN = ide_irq_invoked = 0x0);
	
	// Setup SCSI Packet:
	atapi_packet[ 0] = ATAPI_CMD_READ;
	atapi_packet[ 1] = 0x0;
	atapi_packet[ 2] = (lba >> 24) & 0xFF;
	atapi_packet[ 3] = (lba >> 16) & 0xFF;
	atapi_packet[ 4] = (lba >> 8) & 0xFF;
	atapi_packet[ 5] = (lba >> 0) & 0xFF;
	atapi_packet[ 6] = 0x0;
	atapi_packet[ 7] = 0x0;
	atapi_packet[ 8] = 0x0;
	atapi_packet[ 9] = numsects;
	atapi_packet[10] = 0x0;
	atapi_packet[11] = 0x0;
	
	// Select the drive:
	ide_write(channel, ATA_REG_HDDEVSEL, slavebit << 4);
	
	// Delay 400 nanoseconds for select to complete:
	for(int i = 0; i < 4; i++)
		ide_read(channel, ATA_REG_ALTSTATUS); // Reading the Alternate Status port wastes 100ns
	
	// Inform the Controller that we use PIO mode:
	ide_write(channel, ATA_REG_FEATURES, 0); // PIO mode
	
	// Tell the Controller the size of buffer:
	ide_write(channel, ATA_REG_LBA1, (words * 2) & 0xFF); // Lower Byte of Sector Size
	ide_write(channel, ATA_REG_LBA2, (words * 2) >> 8); // Upper Byte of Sector Size

	// Send the Packet Command:
	ide_write(channel, ATA_REG_COMMAND, ATA_CMD_PACKET); // Send the Command

	// Waiting for the driver to finish or return an error code:
	err = ide_polling(channel, 1);
	if (err)
		return err; // Polling and return if error

	// Sending the packet data:
	asm("rep	outsw" : : "c"(6), "d"(bus), "S"(atapi_packet)); // Send Packet Data
	
	// Receiving Data:
	for (int i = 0; i < numsects; i++)
	{
		ide_wait_irq();	// Wait for an IRQ.
		err = ide_polling(channel, 1);
		if (err)
			return err;	// Polling and return if error.
		asm("pushw %es");
		asm("mov %%ax, %%es"::"a"(selector));
		asm("rep insw"::"c"(words), "d"(bus), "D"(edi));// Receive Data.
		asm("popw %es");
		edi += (words * 2);
	}
	
	// Wait for an IRQ:
	ide_wait_irq();
 
	// Wait for BSY & DRQ to clear:
	while (ide_read(channel, ATA_REG_STATUS) & (ATA_SR_BSY | ATA_SR_DRQ));
	
	return 0; // Successfull
}

u8 ide_read_sectors(u8 drive, u8 numsects, u32 lba, u16 es, u32 edi)
{
 
	// Check if the drive presents:
	if (drive > 3 || ide_devices[drive].reserved == 0)
		return 0x1;		// Drive Not Found!
	// Check if inputs are valid:
	else if (((lba + numsects) > ide_devices[drive].size) && (ide_devices[drive].type == IDE_ATA))
		return 0x2; // Seeking to invalid position
	
	// Read in PIO Mode through Polling & IRQs:
	u8 err;
	if (ide_devices[drive].type == IDE_ATA)
		err = ide_ata_access(ATA_READ, drive, lba, numsects, es, edi);
	else if (ide_devices[drive].type == IDE_ATAPI)
		for (u8 i = 0; i < numsects; i++)
			err = ide_atapi_read(drive, lba + i, 1, es, edi + (i*2048));
	return ide_print_error(drive, err);
}

u8 ide_write_sectors(u8 drive, u8 numsects, u32 lba, u16 es, u32 edi)
{
	// Check if the drive presents:
	if (drive > 3 || ide_devices[drive].reserved == 0)
		return 0x1;		// Drive Not Found!
	// Check if inputs are valid:
	else if (((lba + numsects) > ide_devices[drive].size) && (ide_devices[drive].type == IDE_ATA))
		return 0x2;							// Seeking to invalid position.
  
	// Read in PIO Mode through Polling & IRQs:
	u8 err;
	if (ide_devices[drive].type == IDE_ATA)
		err = ide_ata_access(ATA_WRITE, drive, lba, numsects, es, edi);
	else if (ide_devices[drive].type == IDE_ATAPI)
		err = 4; // Write-Protected.
	return ide_print_error(drive, err);
}

u8 ide_atapi_eject(u8 drive) {
	u32	channel		= ide_devices[drive].channel;
	u32	slavebit		= ide_devices[drive].drive;
	u32	bus		= channels[channel].base;
	u8  err = 0;
	ide_irq_invoked = 0;
 
	// Check if the drive presents:
	if (drive > 3 || ide_devices[drive].reserved == 0)
		return 0x1;		// Drive Not Found!
	// Check if drive isn't ATAPI:
	else if (ide_devices[drive].type == IDE_ATA)
		return 20;			// Command Aborted.
	// Eject ATAPI Driver:
	else {
		// Enable IRQs:
		ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN = ide_irq_invoked = 0x0);
 
		// Setup SCSI Packet:
		atapi_packet[ 0] = ATAPI_CMD_EJECT;
		atapi_packet[ 1] = 0x00;
		atapi_packet[ 2] = 0x00;
		atapi_packet[ 3] = 0x00;
		atapi_packet[ 4] = 0x02;
		atapi_packet[ 5] = 0x00;
		atapi_packet[ 6] = 0x00;
		atapi_packet[ 7] = 0x00;
		atapi_packet[ 8] = 0x00;
		atapi_packet[ 9] = 0x00;
		atapi_packet[10] = 0x00;
		atapi_packet[11] = 0x00;
 
		// Select the Drive:
		ide_write(channel, ATA_REG_HDDEVSEL, slavebit << 4);
 
		// Delay 400 nanosecond for select to complete:
		for(int i = 0; i < 4; i++)
			ide_read(channel, ATA_REG_ALTSTATUS); // Reading Alternate Status Port wastes 100ns.
 
		// Send the Packet Command:
		ide_write(channel, ATA_REG_COMMAND, ATA_CMD_PACKET);		// Send the Command.
 
		// Waiting for the driver to finish or invoke an error:
		err = ide_polling(channel, 1);				// Polling and stop if error.
 
		// Sending the packet data:
		asm("rep	outsw"::"c"(6), "d"(bus), "S"(atapi_packet));// Send Packet Data
		ide_wait_irq();						// Wait for an IRQ.
		err = ide_polling(channel, 1);				// Polling and get error code.
		if (err == 3) err = 0; // DRQ is not needed here.
		
		return ide_print_error(drive, err); // Return;
	}
}
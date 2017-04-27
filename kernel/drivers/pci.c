#include <drivers/pci.h>
#include <arch/x86/ports.h>

#include <string.h>
#include <stdio.h>

u8 pci_config_read_byte(u8 bus, u8 device, u8 function, u8 offset)
{
	/* write out the address */
	outl(0xCF8, (	((u32)bus      << 16) |  // Bus Number
							((u32)device   << 11) |  // Device Number
							((u32)function << 8 ) |  // Function Number
							(offset & 0xfc)            |  // Register Number 11111100 (last 2 bits are not important because 32bits get readed)
							((u32)0x80000000 )		  // Enable bit (= 1)
						 ));
						 
	/* read in the data */
	return (u8)((inl(0xCFC) >> ((offset & 3) * 8)) & 0xFF);
}

u16 pci_config_read_word(u8 bus, u8 device, u8 function, u8 offset)
{
	/* write out the address */outl(0xCF8, (	((u32)bus      << 16) |  // Bus Number
							((u32)device   << 11) |  // Device Number
							((u32)function << 8 ) |  // Function Number
							(offset & 0xfc)            |  // Register Number 11111100 (last 4 bits are not important because 32bits get readed)
							((u32)0x80000000 )		  // Enable bit (= 1)
						 ));	 
	/* read in the data */
	return (u16)((inl(0xCFC) >> ((offset & 2) * 8)) & 0xFFFF);
}

u32 pci_config_read_long(u8 bus, u8 device, u8 function, u8 offset)
{
	/* write out the address */
	outl(0xCF8, (	((u32)bus      << 16) |  // Bus Number
							((u32)device   << 11) |  // Device Number
							((u32)function << 8 ) |  // Function Number
							(offset & 0xfc)            |  // Register Number 11111100 (last 4 bits are not important because 32bits get readed)
							((u32)0x80000000 )		  // Enable bit (= 1)
						 ));		 
	/* read in the data */
	return inl(0xCFC);
}

void pci_check_function(u8 bus, u8 device, u8 function)
{
	u8 baseClass = pci_config_read_byte(bus, device, function, 0xB);
	u8 subClass = pci_config_read_byte(bus, device, function, 0xA);
	
	char buffer[256];
	puts("\nPCI Device:\n");
	printf("Base Class: %x\n", baseClass);
	printf("Sub Class: %x\n", subClass);
}

void pci_check_device(u8 bus, u8 device)
{
	u8 function = 0;
	
	 // Vendor ID is a word located at 0x0 in the PCI header
	u16 vendor_id = pci_config_read_word(bus, device, function, 0);	
	if(vendor_id == 0xFFFF) // There are no vendors == 0xFFFF => Device doesn't exist
		return;
	
	pci_check_function(bus, device, function);
	
	// Header type is a byte located at 0xE in the PCI header
	u8 header_type = pci_config_read_byte(bus, device, function, 0xE);
	
	// Check for multi-function device
	if((header_type & 0x80) != 0)
		for(function = 1; function < 8; function++)
			if(pci_config_read_word(bus, device, function, 0) != 0xFFFF)
				pci_check_function(bus, device, function);
}
 
void pci_check_all_busses()
{
	u8 device;
	for(u32 bus = 0; bus < 256; bus++)
		for(device = 0; device < 32; device++)
			pci_check_device((u8)bus, device);
}


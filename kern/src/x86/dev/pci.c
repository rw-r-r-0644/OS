#include <dev/pci.h>
#include <x86/cpu.h>

#include <string.h>
#include <stdio.h>

uint8_t pci_config_read_byte(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset)
{
	// Write out the address
	outl(0xCF8, (	((uint32_t)bus      << 16)	|  // Bus Number
					((uint32_t)device   << 11)	|  // Device Number
					((uint32_t)function << 8 )	|  // Function Number
					(offset & 0xfc)			|  // Register Number 11111100 (last 2 bits are not important because 32bits get readed)
					((uint32_t)0x80000000 )		   // Enable bit (= 1)
				));

	// Read in the data
	return (uint8_t)((inl(0xCFC) >> ((offset & 3) * 8)) & 0xFF);
}

uint16_t pci_config_read_word(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset)
{
	// Write out the address
	outl(0xCF8, (	((uint32_t)bus      << 16)	|  // Bus Number
					((uint32_t)device   << 11)	|  // Device Number
					((uint32_t)function << 8 )	|  // Function Number
					(offset & 0xfc)			|  // Register Number 11111100 (last 4 bits are not important because 32bits get readed)
					((uint32_t)0x80000000 )		   // Enable bit (= 1)
				));

	// Read in the data
	return (uint16_t)((inl(0xCFC) >> ((offset & 2) * 8)) & 0xFFFF);
}

uint32_t pci_config_read_long(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset)
{
	// Write out the address
	outl(0xCF8, (	((uint32_t)bus      << 16)	|  // Bus Number
					((uint32_t)device   << 11)	|  // Device Number
					((uint32_t)function << 8 )	|  // Function Number
					(offset & 0xfc)			|  // Register Number 11111100 (last 4 bits are not important because 32bits get readed)
					((uint32_t)0x80000000 )		   // Enable bit (= 1)
				));
						 
	// Read in the data
	return inl(0xCFC);
}

void pci_check_function(uint8_t bus, uint8_t device, uint8_t function)
{
	uint8_t baseClass = pci_config_read_byte(bus, device, function, 0xB);
	uint8_t subClass = pci_config_read_byte(bus, device, function, 0xA);
	
	char buffer[256];
	printf("Found PCI Device: Base Class: %x, Sub Class: %x\n", baseClass, subClass);
}

void pci_check_device(uint8_t bus, uint8_t device)
{
	uint8_t function = 0;
	
	 // Vendor ID is a word located at 0x0 in the PCI header
	uint16_t vendor_id = pci_config_read_word(bus, device, function, 0);	
	if(vendor_id == 0xFFFF) // There are no vendors == 0xFFFF => Device doesn't exist
		return;
	
	pci_check_function(bus, device, function);
	
	// Header type is a byte located at 0xE in the PCI header
	uint8_t header_type = pci_config_read_byte(bus, device, function, 0xE);
	
	// Check for multi-function device
	if((header_type & 0x80) != 0)
		for(function = 1; function < 8; function++)
			if(pci_config_read_word(bus, device, function, 0) != 0xFFFF)
				pci_check_function(bus, device, function);
}
 
void pci_check_all_busses()
{
	uint8_t device;
	for(uint32_t bus = 0; bus < 256; bus++)
		for(device = 0; device < 32; device++)
			pci_check_device((uint8_t)bus, device);
}


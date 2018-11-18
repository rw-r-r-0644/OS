# OS Main Makefile

ROOTFS		:= root
OUTPUT		:= os.iso

Q := @
MAKEFLAGS	+= --no-print-directory
MAKE		:= make
TARGETS		:= kern initrd

# Compiler settings
PREFIX		:= i686-elf-
GDB			:= $(PREFIX)gdb

rebuild:
	@echo "--> Rebuilding OS"
	@for dir in $(TARGETS); do $(MAKE) -C $$dir ROOTFS=$(abspath $(ROOTFS)) rebuild; done
	@echo "--> Generating $(OUTPUT)"
	@grub-mkrescue $(ROOTFS) -o $(OUTPUT)
	@echo "--> Build finished"

default:
	@echo "--> Building OS"
	@for dir in $(TARGETS); do $(MAKE) -C $$dir ROOTFS=$(abspath $(ROOTFS)) default ; done
	@echo "--> Generating $(OUTPUT)"
	@grub-mkrescue $(ROOTFS) -o $(OUTPUT)
	@echo "--> Build finished"

clean:
	@echo "--> Cleaning OS"
	@for dir in $(TARGETS); do $(MAKE) -C $$dir ROOTFS=$(abspath $(ROOTFS)) clean; done
	@rm $(OUTPUT)
	@echo "--> Build finished"
	
debug:
	@echo "--> Building OS (debug)"
	@for dir in $(TARGETS); do $(MAKE) -C $$dir ROOTFS=$(abspath $(ROOTFS)) debug; done
	@echo "--> Generating $(OUTPUT)"
	@grub-mkrescue $(ROOTFS) -o $(OUTPUT)
	@echo "--> Build finished"
	@echo "--> Starting emulator and debugger"
	@qemu-system-i386 -s -cdrom $(OUTPUT) -m 512M -d guest_errors,int &
	@$(GDB) -ex "target remote localhost:1234" -ex "symbol-file $(ROOTFS)/boot/kernel"

run: rebuild
	@echo "--> Starting emulator"
	@qemu-system-i386 -cdrom $(OUTPUT) -m 512M

# OS Main Makefile

OUTPUT		:= os.iso

Q := @
MAKEFLAGS	+= --no-print-directory
MAKE		:= make
TARGETS		:= libc kernel

# Compiler settings
PREFIX		:= i386-elf-
GDB			:= $(PREFIX)gdb


rebuild:
	@echo "--> Rebuilding OS"
	@for dir in $(TARGETS); do $(MAKE) -C $$dir rebuild; done
	@echo "--> Generating $(OUTPUT)"
	@cp kernel/kernel.elf isofs/boot/kernel.elf
	@grub-mkrescue isofs -o $(OUTPUT)
	@echo "--> Build finished"

default:
	@echo "--> Building OS"
	@for dir in $(TARGETS); do $(MAKE) -C $$dir default; done
	@echo "--> Generating $(OUTPUT)"
	@cp kernel/kernel.elf isofs/boot/kernel.elf
	@grub-mkrescue isofs -o $(OUTPUT)
	@echo "--> Build finished"

clean:
	@echo "--> Cleaning OS"
	@for dir in $(TARGETS); do $(MAKE) -C $$dir clean; done
	@rm $(OUTPUT)
	@echo "--> Build finished"
	
debug:
	@echo "--> Building OS (debug)"
	@for dir in $(TARGETS); do $(MAKE) -C $$dir debug; done
	@echo "--> Generating $(OUTPUT)"
	@cp kernel/kernel.elf isofs/boot/kernel.elf
	@grub-mkrescue isofs -o $(OUTPUT)
	@echo "--> Build finished"
	@echo "--> Starting emulator and debugger"
	@qemu-system-i386 -s -cdrom $(OUTPUT) -m 512M -d guest_errors,int &
	@$(GDB) -ex "target remote localhost:1234" -ex "symbol-file kernel/kernel.elf"

run: rebuild
	@echo "--> Starting emulator"
	@qemu-system-i386 -cdrom $(OUTPUT) -m 512M

# OS Kernel Makefile
MAKEFLAGS	+= --no-print-directory

ROOTFS		?= .
TARGET		:= $(ROOTFS)/boot/kernel

# Compiler settings
PREFIX		:= i686-elf-
CC			:= $(PREFIX)gcc
GDB			:= $(PREFIX)gdb
LD			:= $(PREFIX)ld
AS			:= nasm

# Autogenerate a list of source files
SOURCE		:= src
SFILES		:= $(shell find $(SOURCE) -name '*.S')
NFILES		:= $(shell find $(SOURCE) -name '*.s')
CFILES		:= $(shell find $(SOURCE) -name '*.c')

# Generate object files from source files
BUILD		:= build
OBJECTS		:=	$(patsubst $(SOURCE)/%.S,$(BUILD)/%.S.o,$(SFILES)) \
				$(patsubst $(SOURCE)/%.s,$(BUILD)/%.s.o,$(NFILES)) \
				$(patsubst $(SOURCE)/%.c,$(BUILD)/%.c.o,$(CFILES))

# Include kernel and libc headers
INCLUDES		:=	-I inc -I inc/lib -I inc/x86

# Build flags
CFLAGS		:=	-g -m32 \
				-ffreestanding -fstack-protector -fno-exceptions \
				-std=gnu11 \
				-Wall -Wextra -Wno-unused ${INCLUDES}

ASFLAGS		:=	-g -m32 $(INCLUDES)

.PHONY: rebuild debug default clean

# Rebuild by default
rebuild: clean default

# Debug
debug:
	$(info -> Building kernel (debug))
	@mkdir -p $(BUILD)
	@$(MAKE) $(TARGET_DBG)

# Create build directory
default:
	$(info -> Building kernel)
	@find $(SOURCE) -type d | sed -e "s?$(SOURCE)?$(BUILD)?" | xargs mkdir -p
	@$(MAKE) $(TARGET)

# Build target
$(TARGET): $(OBJECTS)
	$(info LD $@)
	@$(LD) -T link.ld -o $@ $^

# Build target (debug) - no changes for now
$(TARGET_DBG): $(TARGET)

# Generic rules for wildcards
# To make an object, always compile from its .c
$(BUILD)/%.c.o: $(SOURCE)/%.c
	$(info CC $@)
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.S.o: $(SOURCE)/%.S
	$(info AS $@)
	@$(CC) $(ASFLAGS) -c $< -o $@

$(BUILD)/%.s.o: $(SOURCE)/%.s
	$(info NASM $@)
	@$(AS) $(ASFLAGS) $< -f elf -o $@

clean:
	$(info -> Cleaning kernel)
	@rm -rf $(TARGET) $(BUILD)

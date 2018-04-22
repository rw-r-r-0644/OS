# OS Kernel Makefile

Q := @
MAKEFLAGS += --no-print-directory

TARGET		:= $(ROOTFS)/boot/kernel

# Compiler settings
PREFIX		:= i686-elf-
CC			:= $(PREFIX)gcc
GDB			:= $(PREFIX)gdb
LD			:= $(PREFIX)ld
AS			:= nasm

# Autogenerate a list of source files
SRC_DIR		:= src
C_SOURCES	:= $(shell find $(SRC_DIR) -name '*.c')
A_SOURCES	:= $(shell find $(SRC_DIR) -name '*.s')

# Generate object files from source files
OUT_DIR		:= build
OBJS		:= $(patsubst $(SRC_DIR)/%.s,$(OUT_DIR)/%.o,$(A_SOURCES))  $(patsubst $(SRC_DIR)/%.c,$(OUT_DIR)/%.o,$(C_SOURCES))

# Include kernel and libc headers
INC_DIR		:=	-I inc -I inc/lib

# Build flags
CFLAGS		:=	-g -m32 \
				-ffreestanding -fstack-protector -fno-exceptions \
				-std=gnu11 \
				-Wall -Wextra -Wno-unused ${INC_DIR}

# Rebuild by default
rebuild: clean default

# Debug
debug:
	$(info -> Building kernel (debug))
	@mkdir -p $(OUT_DIR)
	@$(MAKE) $(TARGET_DBG)

# Create build directory
default:
	$(info -> Building kernel)
	@find $(SRC_DIR) -type d | sed -e "s?$(SRC_DIR)?$(OUT_DIR)?" | xargs mkdir -p
	@$(MAKE) $(TARGET)

# Build target
$(TARGET): $(OBJS)
	$(info LD $@)
	@$(LD) -T link.ld -o $@ $^

# Build target (debug) - no changes for now
$(TARGET_DBG): $(TARGET)

# Generic rules for wildcards
# To make an object, always compile from its .c
$(OUT_DIR)/%.o: $(SRC_DIR)/%.c
	$(info CC $@)
	@$(CC) $(CFLAGS) -c $< -o $@

$(OUT_DIR)/%.o: $(SRC_DIR)/%.s
	$(info AS $@)
	@$(AS) $< -f elf -o $@

clean:
	$(info -> Cleaning kernel)
	@rm -rf $(TARGET) $(OUT_DIR)

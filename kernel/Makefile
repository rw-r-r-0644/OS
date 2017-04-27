# OS Makefile

Q := @
MAKEFLAGS += --no-print-directory

TARGET		:= kernel.elf

# Autogenerate a list of source files
SOURCEDIR	:= .
C_SOURCES	:= $(shell find $(SOURCEDIR) -name '*.c')
A_SOURCES	:= $(shell find $(SOURCEDIR) -name '*.s')
C_HEADERS	:= $(shell find $(SOURCEDIR) -name '*.h')

# Generate object files from source files
BUILD_DIR	:= build
C_OBJ		:= $(C_SOURCES:.c=.o)
A_OBJ		:= $(A_SOURCES:.s=.o)

# Compiler settings
PREFIX		:= i386-elf-
CC			:= $(PREFIX)gcc
GDB			:= $(PREFIX)gdb
LD			:= $(PREFIX)ld
AS			:= nasm

# Include kernel and libc headers
INCLUDES	:=	-I include \
				-I ../libc/include

# Build flags
CFLAGS		:=	-g -m32 \
				-ffreestanding -fstack-protector -fno-exceptions \
				-Wall -Wextra -Wno-unused ${INCLUDES}

# Rebuild by default
rebuild: clean default

# Debug
debug:
	@echo "-> Building Kernel (debug)"
	@mkdir -p $(BUILD_DIR)
	@$(MAKE) $(TARGET_DBG)

# Create build directory
default:
	@echo "-> Building Kernel"
	@mkdir -p $(BUILD_DIR)
	@$(MAKE) $(TARGET)

# Build target
$(TARGET): $(A_OBJ) $(C_OBJ)
	@echo "LD $@"
	@$(LD) -T link.ld -o $@ $(addprefix $(BUILD_DIR)/, $(notdir $^)) ../libc/libk.a

# Build target (debug) - no changes for now
$(TARGET_DBG): $(TARGET)

# Generic rules for wildcards
# To make an object, always compile from its .c
%.o: %.c $(C_HEADERS)
	@echo "CC $<"
	@$(CC) $(CFLAGS) -c $< -o $(BUILD_DIR)/$(notdir $@)

%.o: %.s
	@echo "AS $<"
	@$(AS) $< -f elf -o $(BUILD_DIR)/$(notdir $@)

clean:
	@echo "-> Cleaning Kernel"
	@rm -rf $(TARGET) $(BUILD_DIR)

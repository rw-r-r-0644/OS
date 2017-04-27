# OS libc/libk Makefile

Q := @
MAKEFLAGS	+= --no-print-directory

# Build directory
BUILD_DIR	:= build

# Autogenerate a list of source files
SOURCEDIR	:= .
CFILES		:= $(shell find $(SOURCEDIR) -name '*.c')

# Generate object files from source files
OBJ			:= $(CFILES:.c=.o)
LIBK_OBJ	:= $(OBJ:.o=.libk.o)
LIBC_OBJ	:= $(OBJ:.o=.libc.o)

# Compiler settings
PREFIX		:= i386-elf-
CC			:= $(PREFIX)gcc
GDB			:= $(PREFIX)gdb
AR			:= $(PREFIX)ar

# Include kernel and libs headers
INCLUDES	:=	-I ../kernel/include \
				-I include
			
# Build flags
CFLAGS		:=	-g -m32 \
				-ffreestanding -fno-exceptions \
				-Wall -Wextra  $(INCLUDES)

CFLAGS_LIBK	:= $(CFLAGS) -D__is_libk
CFLAGS_LIBC	:= $(CFLAGS) -D__is_libc

rebuild: clean default

# Create build directory
default:
	@mkdir -p $(BUILD_DIR)
	@echo "-> Building libk"
	@$(MAKE) libk.a
	@echo "-> Building libc"
	@$(MAKE) libc.a

debug: default

libk.a: $(LIBK_OBJ)
	@echo "AR $@"
	@$(AR) rcs $@ $(BUILD_DIR)/*.libk.o

libc.a: $(LIBC_OBJ)
	@echo "AR $@"
	@$(AR) rcs $@ $(BUILD_DIR)/*.libc.o

# To make an object, always compile from its .c
%.libk.o: %.c
	@echo "CC $<"
	@$(CC) $(CFLAGS_LIBK) -c $< -o $(BUILD_DIR)/$(notdir $@)

%.libc.o: %.c
	@echo "CC $<"
	@$(CC) $(CFLAGS_LIBC) -c $< -o $(BUILD_DIR)/$(notdir $@)

clean:
	@echo "-> Cleaning libk/libc"
	@rm -rf $(BUILD_DIR)/*.o *.a

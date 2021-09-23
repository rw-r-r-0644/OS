# OS Initrd Makefile

Q := @
MAKEFLAGS += --no-print-directory

TARGET		:= $(ROOTFS)/boot/initrd

HOSTCC		:= gcc
OUT_DIR		:= build
INITRD_DIR	:= fs

.PHONY: rebuild debug default clean

# Rebuild by default
rebuild: clean default

debug: default
default:
	$(info -> Generating initrd)
	@mkdir -p $(OUT_DIR)
	@$(MAKE) $(TARGET)

$(TARGET): $(INITRD_DIR) $(OUT_DIR)/mkinitrd
	$(info INITRD $@)
	@$(OUT_DIR)/mkinitrd $(INITRD_DIR) $@

$(OUT_DIR)/mkinitrd: mkinitrd.c
	$(info HOSTCC $@)
	@$(HOSTCC) $< -o $@

clean:
	$(info -> Cleaning initrd)
	@rm -rf $(TARGET) $(OUT_DIR)
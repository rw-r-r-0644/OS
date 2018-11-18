#pragma once

#include <fs/fs.h>
#include <stdint.h>

// Mount an initrd fs
fs_node_t * initrd_mount(fs_node_t** mount_path, uint8_t * buff);

// Initrd file function prototypes
static uint32_t initrd_read(fs_node_t * node, uint32_t offset, uint32_t size, uint8_t * buffer);
static struct dirent * initrd_readdir(fs_node_t * node, uint32_t index);
static fs_node_t * initrd_finddir(fs_node_t * node, char * name);
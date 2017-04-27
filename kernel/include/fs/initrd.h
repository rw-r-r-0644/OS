#pragma once

#include <fs/fs.h>
#include <stdint.h>

// Mount an initrd fs
fs_node_t * initrd_mount(fs_node_t** mount_path, u8 * buff);

// Initrd file function prototypes
static u32 initrd_read(fs_node_t * node, u32 offset, u32 size, u8 * buffer);
static struct dirent * initrd_readdir(fs_node_t * node, u32 index);
static fs_node_t * initrd_finddir(fs_node_t * node, char * name);
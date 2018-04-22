/*
 * initrd.c
 * Based on JamesM work
 * needs a rewrite too someday...
*/

#include <fs/initrd.h>

#include <lib/string.h>
#include <lib/stdint.h>
#include <lib/stdlib.h>
#include <lib/utils.h>

#include <utils/logger.h>

typedef struct
{
	char magic[8];
	uint32_t entry_count;
} _packed initrd_header_t;

typedef struct
{
	char name[128];
	uint32_t  offset;
	uint32_t  length;
} _packed initrd_file_desc_t;

initrd_header_t * initrd_header;	// Initrd header
initrd_file_desc_t * initrd_descs;	// Initrd descriptors array
u8 * initrd_file_area;				// Initrd file area

fs_node_t * initrd_root;			// Initrd Root directory node
fs_node_t * initrd_dev;				// Directory node for /dev
fs_node_t * root_nodes;				// List of file nodes
u32 nroot_nodes;					// Number of file nodes

fs_node_t * initrd_mount(fs_node_t** mount_path, u8 * buff)
{	
	// Let's hope the kernel didn't get hacked/messed that early and just skip checks...
	initrd_header		= (initrd_header_t *)buff;
	initrd_descs		= (initrd_file_desc_t *)((u32)buff + sizeof(initrd_header_t));
	initrd_file_area	= (u8 *)((u32)buff + sizeof(initrd_header_t) + (sizeof(initrd_file_desc_t) * initrd_header->entry_count)); 
		
	// Initialise the root directory
	initrd_root = calloc(1, sizeof(fs_node_t));
	strcpy(initrd_root->name, "initrd");
	initrd_root->flags = FS_DIRECTORY;
	initrd_root->readdir = &initrd_readdir;
	initrd_root->finddir = &initrd_finddir;
		
	// Initialise the /dev directory
	initrd_dev = calloc(1, sizeof(fs_node_t));
	strcpy(initrd_dev->name, "dev");
	initrd_dev->flags = FS_DIRECTORY;
	initrd_dev->readdir = &initrd_readdir;
	initrd_dev->finddir = &initrd_finddir;
		
	// Create file nodes
	nroot_nodes = initrd_header->entry_count;
	root_nodes = calloc(1, sizeof(fs_node_t) * nroot_nodes);
		
	for (u32 i = 0; i < nroot_nodes; i++)
	{
		// Create a new file node
		strcpy(root_nodes[i].name, initrd_descs[i].name);
		root_nodes[i].length = initrd_descs[i].length;
		root_nodes[i].inode = i;
		root_nodes[i].flags = FS_FILE;
		root_nodes[i].read = &initrd_read;
	}
		
	*mount_path = initrd_root;
	return initrd_root;
}

static u32 initrd_read(fs_node_t * node, u32 offset, u32 size, u8 * buffer)
{	
	initrd_file_desc_t *desc = &initrd_descs[node->inode];
	
	if (offset > desc->length)
		return 0;
	
	if ((offset + size) > desc->length)
		size = desc->length - offset;
	
	memcpy(buffer, &initrd_file_area[desc->offset + offset], size);
	return size;
}

static struct dirent * initrd_readdir(fs_node_t * node, u32 index)
{
	struct dirent * dir = malloc(sizeof(struct dirent));
	if (node == initrd_root && index == 0)
	{
		strcpy(dir->d_name, "dev");
		dir->d_ino = 0;
		return dir;
	}
	
	// The first item was the /dev entry, shifting all the list after it up by one...
	index--;
	
	if (index >= nroot_nodes)
		return NULL;
	
	strcpy(dir->d_name, root_nodes[index].name);
	dir->d_ino = root_nodes[index].inode;
	return dir;
}

static fs_node_t * initrd_finddir(fs_node_t * node, char * name)
{
	if (node == initrd_root && strcmp(name, "dev") == 0)
		 return initrd_dev;

	for (u32 i = 0; i < nroot_nodes; i++)
		 if (strcmp(name, root_nodes[i].name) == 0)
			  return &root_nodes[i];
	
	// Not found
	return NULL;
}
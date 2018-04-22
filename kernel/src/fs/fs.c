#include <fs/fs.h>
#include <string.h>

fs_node_t *fs_root = 0; // The root of the filesystem.

u32 read_fs(fs_node_t * node, u32 offset, u32 size, u8 * buffer)
{
	if (!node)
		return 0;
	
	if (node->read)
		return node->read(node, offset, size, buffer);
	else
		return 0;
}

u32 write_fs(fs_node_t * node, u32 offset, u32 size, u8 * buffer)
{
	if (!node)
		return 0;
	
	if (node->write)
		return node->write(node, offset, size, buffer);
	else
		return 0;
}

void open_fs(fs_node_t * node, u32 flags)
{
	if (!node)
		return;
	
	if (node->open)
		return node->open(node, flags);
}

void close_fs(fs_node_t * node)
{
	if (!node)
		return;
	
	if(node->close)
		return node->close(node);
}

struct dirent * readdir_fs(fs_node_t * node, u32 index)
{
	if (!node)
		return NULL;
	
	if ((node->readdir) && (node->flags & FS_DIRECTORY))
		return node->readdir(node, index);
	else
		return NULL;
}

fs_node_t * finddir_fs(fs_node_t * node, char * name)
{
	if (!node)
		return NULL;
	
	if ((node->finddir) && (node->flags & FS_DIRECTORY))
		return node->finddir(node, name);
	else
		return NULL;
}
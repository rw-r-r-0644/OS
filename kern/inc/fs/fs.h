/*
 * fs.h
 *
 * Based on JamesM's work
 * It would be nice to get it rewritten sometime
*/

#pragma once

#include <stdint.h>

// FS Flags
#define FS_FILE        0x01
#define FS_DIRECTORY   0x02
#define FS_CHARDEVICE  0x03
#define FS_BLOCKDEVICE 0x04
#define FS_PIPE        0x05
#define FS_SYMLINK     0x06
#define FS_MOUNTPOINT  0x08

// Needed for function prototypes
struct fs_node;

// Filesystem function prototypes
typedef uint32_t (*read_type_t)(struct fs_node * node, uint32_t offset, uint32_t size, uint8_t * buffer);
typedef uint32_t	(*write_type_t)(struct fs_node * node, uint32_t offset, uint32_t size, uint8_t * buffer);
typedef void (*open_type_t)(struct fs_node * node, uint32_t flags);
typedef void (*close_type_t)(struct fs_node * node);
typedef struct dirent * (*readdir_type_t)(struct fs_node * node, uint32_t index);
typedef struct fs_node * (*finddir_type_t)(struct fs_node * node, char * name);

// Filesystem node
typedef struct fs_node
{
	char name[128];			// The filename
	uint32_t mask;				// The permissions mask
	uint32_t uid;				// The owning user
	uint32_t gid;				// The owning group
	uint32_t flags;				// The node type
	uint32_t inode;				// This is device-specific - provides a way for a filesystem to identify files.
	uint32_t length;				// Size of the file
	uint32_t impl;				// An implementation-defined number
	read_type_t read;		// File's read function
	write_type_t write;		// File's write function
	open_type_t open;		// File's open function
	close_type_t close;		// File's close function
	readdir_type_t readdir;	// Directory's read function
	finddir_type_t finddir;	// Directory's find function
	struct fs_node * ptr;	// Used by mountpoints and symlinks
} fs_node_t;

// Directory entry
struct dirent
{
	uint32_t d_ino;        // Inode number
	char d_name[128]; // Filename
};

// Root of the filesystem
extern fs_node_t *fs_root;

// Flags for open_fs
#define O_RDONLY	0x0000
#define O_WRONLY	0x0001
#define O_RDWR		0x0002
/*
TODO: Implement the other access flags...

#define O_ACCMODE	0x0003
#define O_CREAT		0x0100
#define O_EXCL		0x0200
#define O_NOCTTY	0x0400
#define O_TRUNC		0x0800
#define O_APPEND	0x1000
#define O_NONBLOCK	0x2000
*/

/*
 * Standard read/write/open/close functions
 * Suffixed with _fs to distinguish them from the read/write/open/close which deal with file descriptors
 */
uint32_t read_fs(fs_node_t * node, uint32_t offset, uint32_t size, uint8_t * buffer);
uint32_t write_fs(fs_node_t * node, uint32_t offset, uint32_t size, uint8_t * buffer);
void open_fs(fs_node_t * node, uint32_t flags);
void close_fs(fs_node_t * node);
struct dirent * readdir_fs(fs_node_t * node, uint32_t index);
fs_node_t * finddir_fs(fs_node_t * node, char * name);


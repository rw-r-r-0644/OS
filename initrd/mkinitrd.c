/*
 * mkinitrd.c
 * Creates an initrd file from a directory
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <assert.h>

#pragma pack(1)


typedef struct
{
	char magic[8];
	uint32_t  entry_count;
} initrd_header_t;

typedef struct
{
	char name[128];
	uint32_t  offset;
	uint32_t  lenght;
} initrd_file_desc_t;

/*
 * -- initrd layout --
 * header
 * descriptors
 * raw files
*/

int main(int argc, char *argv[])
{
	printf("\nmkinitrd v0.1\n");
	if (argc < 2)
	{
		printf("Usage: mkinitrd [initrd directory] [initrd file]\n");
		return -1;
	}
	
	DIR *dir = opendir(argv[1]);
	assert(dir != NULL);
	rewinddir(dir);

	int initrd_fd = open(argv[2], O_RDWR | O_CREAT | O_SYNC, 0666);
	assert(initrd_fd > 0);
	lseek(initrd_fd, 0, SEEK_SET);

	/* Create the header */
	printf("\n\nWriting the header...\n");
	
	// Get file count
	uint32_t file_count = 0;
	struct dirent *ent;
	
	while((ent = readdir(dir)) != NULL)
		if (ent->d_name[0] != '.')
			file_count++;
	
	// Create an header
	initrd_header_t * header = calloc(1, sizeof(initrd_header_t));
	strncpy(header->magic, "INITRD!", 8);
	header->entry_count = file_count;
	
	printf("- magic: %s\n", header->magic);
	printf("- entry count: %u\n", header->entry_count);
	
	// Write the header
	write(initrd_fd, header, sizeof(initrd_header_t));
	free(header);
	
	// Rewind the directory
	rewinddir(dir);
	
	/* Write the files */
	printf("\nWriting files to initrd...\n");
	
	uint32_t current_offset = 0;
	while ((ent = readdir(dir)) != NULL)
	{
		if (ent->d_name[0] == '.')
		{
			ent = readdir(dir);
			continue;
		}
		
		printf("\nWriting %s ", ent->d_name);

		// Open the file
		char * f_path = malloc(strlen(argv[1]) + strlen(ent->d_name) + 30);
		sprintf(f_path, "%s/%s", argv[1], ent->d_name);
		int current_fd = open(f_path, O_RDONLY | O_SYNC);
		free(f_path);

		// Get file's size
		uint32_t f_lenght = lseek(current_fd, 0, SEEK_END);
		lseek(current_fd, 0, SEEK_SET);
		
		// Create a file descriptor
		initrd_file_desc_t * desc = malloc(sizeof(initrd_file_desc_t));
		memset(desc, 0, sizeof(initrd_file_desc_t));

		strncpy(desc->name, ent->d_name, 128);
		desc->offset = current_offset;
		desc->lenght = f_lenght;
		printf("(lenght: %u,", desc->lenght);
		printf(" offset: %u)\n", desc->offset);

		// Write the descriptor
		printf("- writing descriptor at offset %lu\n", lseek(initrd_fd, 0, SEEK_CUR));								// Write the descriptor at current offset
		write(initrd_fd, desc, sizeof(initrd_file_desc_t));
		
		// Write the file
		uint32_t desc_pos = lseek(initrd_fd, 0, SEEK_CUR);															// Save next descriptor's offset
		uint32_t f_offset = sizeof(initrd_header_t) + (sizeof(initrd_file_desc_t) * file_count) + current_offset;	// Calculate file's offset
		lseek(initrd_fd, f_offset, SEEK_SET);																		// Set file's offset
		
		printf("- writing file at offset %lu\n", lseek(initrd_fd, 0, SEEK_CUR));
		sendfile(initrd_fd, current_fd, NULL, f_lenght);															// Write the file
		lseek(initrd_fd, desc_pos, SEEK_SET);																		// Go back to next descriptor's offset
		
		current_offset += f_lenght;																					// Prepare next file's offset
		
		// Cleanup
		free(desc);
		close(current_fd);
	}
	
	close(initrd_fd);
	closedir(dir);
}

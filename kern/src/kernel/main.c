/*
 * kernel.c
 *
 * Main kernel C file
 *
 */

#include <kernel/main.h>
#include <dev/tty.h>
#include <dev/dev.h>

#include <x86/cpu.h>
#include <x86/paging.h>

#include <mm/mm.h>

#include <boot/multiboot2.h>

#include <fs/fs.h>
#include <fs/initrd.h>

#include <lib/string.h>
#include <lib/stdint.h>
#include <lib/stdlib.h>
#include <lib/stdbool.h>
#include <lib/stdio.h>
#include <lib/unistd.h>

#include <utils/logger.h>

/* kernel entrypoint */
void kern_main()
{
	mm_init();
	dev_init();

	// Mount initial filesystem!
	//initrd_mount(&fs_root, mboot_modules[0]);

	// Kernel welcome message
	printf( "\n\nOS KERNEL\n" \
			"Built on %s %s\n\n", __TIME__, __DATE__ );

	// load_program("/dev/initramfs/shell.elf");
	puts("\n\n> ");

	for(;;) asm volatile("hlt");
}

// That's some bad temporary shell there, used for testing... TODO: Remove this horrible code
void user_input(char *input)
{
	if(!strcmp(input, "hlt"))
	{
		puts("Stopping the cpu...\n");
		hlt();
	}
	else if(!strcmp(input, "info"))
	{
		printf("\nOS KERNEL\n");
		printf("Built on %s %s\n", __TIME__, __DATE__);
		printf("Released under GNU GPL\n\n");
	}
	else if(!memcmp(input, "sleep", sizeof("sleep") - 1))
	{
		int n = atoi(&input[6]);
		sleep(n);
	}
	else if(!strcmp(input, "smash stack"))
	{
		char break_me[16];
		char evil[20];
		memset(evil, 1, 20);
		memcpy(break_me, evil, 256);
	}
	else if(!strcmp(input, "crash"))
	{
		int ac = -1;
		ac++;
		int bc = 20;
		int cc = bc / ac;
	}
	else if(!strcmp(input, "clear"))
	{
		tty_clear();
	}
	else if(!memcmp(input, "atoi", sizeof("atoi") - 1))
	{
		int n = atoi(&input[5]);
		printf("N = %d\n", n);
	}
	else if(!strcmp(input, "log"))
	{
		log_printf(LOG_DEBUG, "Debug");
		log_printf(LOG_INFO, "Info");
		log_printf(LOG_WARNING, "Warning");
		log_printf(LOG_ERROR, "Error");
		log_printf(LOG_FATAL, "Fatal");
	}
	else if(!strcmp(input, "heap"))
	{
		uint32_t t1 = timer_get_ticks();

		uint32_t * a = malloc(30);
		uint32_t * b = malloc(70);
			printf("addr a1 = %08x\n", a);
		printf("addr b1 = %08x\n", b);

		free(a);
		free(b);
			a = malloc(120);
		printf("addr a2 = %08x\n", a);
		free(a);

		uint32_t t2 = timer_get_ticks();

		uint32_t score = t2 - t1;
		printf("time: %u\n", score);
	}
	else if(!strcmp(input, "initrd"))
	{
		// list the contents of /
		int i = 0;
		struct dirent *node = 0;
		while ( (node = readdir_fs(fs_root, i)) != 0)
		{
			fs_node_t *fsnode = finddir_fs(fs_root, node->d_name);
			printf("/%s%c\n", node->d_name, (fsnode->flags & FS_DIRECTORY) ? '/' : ' ');
			i++;
		}
	}
	else if(!strcmp(input, "bootinfo"))
	{
		//printf("Booted by: %s\n", mbinfo->boot_loader_name);
	}
	else if(!memcmp(input, "color", sizeof("color") - 1))
	{
		uint16_t color = 0;
		const char * hex_nms = "0123456789abcdef";
		for (int i = 0; i < 16; i++)
			if (input[6] == hex_nms[i])
				color |= vga_color(i, 0);
		for (int i = 0; i < 16; i++)
			if (input[7] == hex_nms[i])
				color |= vga_color(0, 1);

		tty_set_color(color);
	}
	puts("> ");
}

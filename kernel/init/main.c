/*
 * kernel.c
 *
 * Main kernel C file 
 *
 */

#include <init/main.h>
#include <drivers/tty.h>
#include <drivers/pci.h>
#include <drivers/ide.h>

#include <arch/x86/cpu.h>

#include <mem/kmalloc.h>
#include <mem/paging.h>
#include <mem/page_frame.h>
#include <mem/heap.h>

#include <mboot/mboot.h>
#include <mboot/mboot_mods.h>

#include <fs/fs.h>
#include <fs/initrd.h>

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include <utils/logger.h>

multiboot_info_t * mbinfo;

/* early kernel entrypoint */
void kernel_early(u32 mbinfo_ptr)
{
	mbinfo = (multiboot_info_t * )mbinfo_ptr;
	
	// Initialize tty so we get logging
	tty_init();
	
	// Initialize logging
	log_init();
	
	// Setup the gdt
	gdt_init();
	
	// Intialize ISR handlers
	isr_init();
	
	// Initialize paging
	paging_init();
	
	// Initialize pageframe allocation
	pageframe_init(mbinfo->mem_lower + mbinfo->mem_upper);
	
	// Create our own heap
	heap_create(&kernel_heap, 0xC0000000, 0xC0200000, 3);
}

/* main kernel entrypoint */
void kernel_main()
{
	// Intialize IRQ devices
	irq_init();
	
	// Enable IRQs
	sti();
	
	// Map multiboot modules
	mboot_map_mods(mbinfo);
	
	// Mount initial filesystem!
	initrd_mount(&fs_root, mboot_modules[0]);
	
	// Kernel welcome message
	printf( "\n\nOS KERNEL\n" \
			"Built on %s %s\n\n", __TIME__, __DATE__ );

	// load_program("/dev/initramfs/shell.elf");
	puts("\n\n> ");
}

// That's some bad temporary shell there, used for testing... TODO: Remove this horrible code
#define cmd_start if(!1){
#define cmd(str) }else if(strcmp(input,str)==0){
#define cmd_arg(str) }else if(memcmp(input,str,strlen(str))==0){
#define cmd_end }

void user_input(char *input) {
	cmd_start
		cmd("hlt")
			puts("Stopping the cpu...\n");
			hlt();
		cmd("page")
			u32 phys_addr;
			u32* page = kmalloc(1000, true, &phys_addr);
			printf("Allocated a page\n");
			printf("Page: %08x, Physical Address: %08x\n", page, phys_addr);
		cmd("memcmp")
			printf("memcmp\n");
			char a[] = {1,2,3};
			char b[] = {1,2,3,1,1};
			char c[] = {1,2,3,4,5};
			int d = memcmp(a, b, sizeof(a) / sizeof(a[0]));
			int e = memcmp(b, c, sizeof(b) / sizeof(b[0]));
			printf("d=%d e=%d\n", d, e);
		cmd("lspci")
			pci_check_all_busses();
		cmd("info")
			printf("\nOS KERNEL\n");
			printf("Built on %s %s\n", __TIME__, __DATE__);
			printf("Released under GNU GPL\n\n");
		cmd_arg("sleep")
			int n = atoi(&input[6]);
			sleep(n);
		cmd("ide")
			ide_initialize(0x1F0, 0x3F6, 0x170, 0x376, 0x000);
			//ide_read_sectors(1, 1, i, (unsigned int)buf);
		cmd("float")
			float a = 0.5f;
			float b = 0.75f;
			float c = a - b;
			if (c != 0.25f)
				printf("uhmmmm...\n");
			printf("%08x\n", c);
		cmd("smash stack")
			char break_me[16];
			char evil[20];
			memset(evil, 1, 20);
			memcpy(break_me, evil, 256);
		cmd("crash")
			int ac = -1;
			ac++;
			int bc = 20;
			int cc = bc / ac;
		cmd("clear")
			tty_clear_screen();
		cmd_arg("atoi")
			int n = atoi(&input[5]);
			printf("N = %d\n", n);
		cmd("log")
			log_printf(LOG_DEBUG, "Debug");
			log_printf(LOG_INFO, "Info");
			log_printf(LOG_WARNING, "Warning");
			log_printf(LOG_ERROR, "Error");
			log_printf(LOG_FATAL, "Fatal");
		cmd("heap")
			u32 t1 = timer_get_ticks();
		
			u32 * a = heap_alloc(&kernel_heap, 30);
			u32 * b = heap_alloc(&kernel_heap, 70);

			printf("addr a1 = %08x\n", a);
			printf("addr b1 = %08x\n", b);
			
			heap_free(&kernel_heap, a);
			heap_free(&kernel_heap, b);

			a = heap_alloc(&kernel_heap, 120);
			printf("addr a2 = %08x\n", a);
			heap_free(&kernel_heap, a);
						
			u32 t2 = timer_get_ticks();
			
			u32 score = t2 - t1;
			printf("time: %u\n", score);
		cmd("initrd")
			// list the contents of /
			int i = 0;
			struct dirent *node = 0;
			while ( (node = readdir_fs(fs_root, i)) != 0)
			{
				fs_node_t *fsnode = finddir_fs(fs_root, node->d_name);
				printf("/%s%c\n", node->d_name, (fsnode->flags & FS_DIRECTORY) ? '/' : ' ');
				i++;
			}

		cmd("bootinfo")
			printf("Booted by: %s\n", mbinfo->boot_loader_name);
		cmd_arg("color")
			vga_color_t color;
			
			const char * hex_nms = "0123456789abcdef";
			for (int i = 0; i < 16; i++)
				if (input[6] == hex_nms[i])
					color.bg = i;
			for (int i = 0; i < 16; i++)
				if (input[7] == hex_nms[i])
					color.fg = i;
			
			tty_set_color(color);
	cmd_end

	puts("> ");
}

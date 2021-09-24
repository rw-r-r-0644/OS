#include <kernel/main.h>
#include <x86/paging.h>
#include <x86/tty.h>
#include <mm/mm.h>
#include <stdio.h>

void kern_main()
{
	mm_init();
	tty_init();

	printf( "OS KERNEL\n"
			"Built on %s %s\n\n", __TIME__, __DATE__ );

	while (1);
}

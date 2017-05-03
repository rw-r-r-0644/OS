#include <stdint.h>
#include <mboot/mboot.h>

extern void kernel_early();
extern void kernel_main();

/* Reserve some space for the stack */
#define STACK_SIZE		16384
#define STACK_BOTTOM	&stack[0]
#define STACK_TOP		&stack[STACK_SIZE]

u8 stack[STACK_SIZE] __attribute__((aligned(4))) = {0};

/* Entrypoint */
void entrypoint()
{
	// Setup the stack
	asm volatile("movl %0, %%esp"::"r"(STACK_TOP));
	
	// Get multiboot info
	asm volatile("movl %%ebx, %0":"=r"(mbinfo));
	
	// Early kernel entrypoint
	kernel_early();
	 
	// Call the kernel
	kernel_main();
		
	// The kernel should never return
	for(;;);
}
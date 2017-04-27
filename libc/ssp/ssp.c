#include <sys/cdefs.h>
#include <stdint.h>
#include <stdio.h>
 
#define STACK_CHK_GUARD 0x73e23789

uintptr_t __stack_chk_guard = STACK_CHK_GUARD;
 
__attribute__((noreturn))
void __stack_chk_fail(void)
{
	#if defined(__is_libk)
		printf("\nFATAL: kernel stack smashing detected!!");
		printf("\nkernel halted.");
		for(;;);
	#else
		// TODO: Properly terminate userspace program
		for(;;);
	#endif
}
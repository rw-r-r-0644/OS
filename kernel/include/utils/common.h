#pragma once

#include <stdint.h>

extern u32 kernel_start;
	
extern u32 kernel_text_start;
extern u32 kernel_text_end;
	
extern u32 kernel_rodata_start;
extern u32 kernel_rodata_end;

extern u32 kernel_data_start;
extern u32 kernel_data_end;

extern u32 kernel_bss_start;
extern u32 kernel_bss_end;
	
extern u32 kernel_end;
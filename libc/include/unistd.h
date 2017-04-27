#pragma once

#include <sys/cdefs.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

u32 sleep(u32 seconds);
u32 usleep(u32 useconds);

#ifdef __cplusplus
}
#endif
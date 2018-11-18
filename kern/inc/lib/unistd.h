#pragma once

#include <sys/cdefs.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint32_t sleep(uint32_t seconds);
uint32_t usleep(uint32_t useconds);

#ifdef __cplusplus
}
#endif
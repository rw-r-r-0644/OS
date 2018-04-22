#pragma once

#include <stdint.h>

//! Initialize the timer
void init_timer(u32 freq);

//! Get the number of ticks since the start
u32 timer_get_ticks();

//! Wait a number of ticks
void timer_wait(u32 ticks);

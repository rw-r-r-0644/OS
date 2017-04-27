#pragma once

#include <stdint.h>

//! Initialize the timer
void init_timer(uint32_t freq);

//! Get the number of ticks since the start
uint32_t timer_get_ticks();

//! Wait a number of ticks
void timer_wait(uint32_t ticks);

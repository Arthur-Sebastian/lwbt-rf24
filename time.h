/**
@file time.h
@brief Minimal ATmegaX8 timer driver.
@author Artur Miller
@copyright (C) Artur Miller 2023
*/

#ifndef TIME
#define TIME

#include <stdint.h>
#include "config.h"

void tm_init(void);


uint32_t tm_ms(void);


void tm_halt(uint16_t ms);


void tm_reset(void);

#endif

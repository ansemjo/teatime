// Configure the ATtiny417 system clock and RTC
// Copyright (c) 2022 Anton Semjonov
// Licensed under the MIT License

#pragma once

#include <avr/io.h>
#include <avr/interrupt.h>


#if (F_CPU != 10000000L)
#error F_CPU should be defined as 10 MHz
#endif

void setup_10mhz_system_clock();
void setup_crystal_rtc(RTC_PERIOD_t tick_period);
extern void tick();
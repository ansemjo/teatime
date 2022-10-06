// Configure the ATtiny417 system clock and RTC
// Copyright (c) 2022 Anton Semjonov
// Licensed under the MIT License

#pragma once

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>


#if (F_CPU != 10000000L)
#error F_CPU should be defined as 10 MHz
#endif

void setup_system_clock();
void setup_crystal();
void setup_pit_ticks(RTC_PERIOD_t tick_period);
void setup_rtc_seconds();

void run_rtc(uint16_t cnt);
uint16_t halt_rtc();

extern void tick();
extern void second();

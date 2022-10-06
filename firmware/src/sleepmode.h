// Configure and enable the sleep mode between "power-down" and "idle".
// Copyright (c) 2022 Anton Semjonov
// Licensed under the MIT License

#pragma once

#include <avr/io.h>

// enable sleep modes in general
#define sleep_enable() SLPCTRL.CTRLA |= SLPCTRL_SEN_bm;

// configure idle sleep mode (peripherals remain active)
#define sleep_configure_idle() SLPCTRL.CTRLA = SLPCTRL_SMODE_IDLE_gc | (SLPCTRL.CTRLA & SLPCTRL_SEN_bm)

// configure standy sleep mode (RTC remains active)
#define sleep_configure_standby() SLPCTRL.CTRLA = SLPCTRL_SMODE_STDBY_gc | (SLPCTRL.CTRLA & SLPCTRL_SEN_bm)

// configure power-down sleep mode (only PIT can wake)
#define sleep_configure_powerdown() SLPCTRL.CTRLA = SLPCTRL_SMODE_PDOWN_gc | (SLPCTRL.CTRLA & SLPCTRL_SEN_bm)

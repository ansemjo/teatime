// Configure and enable the sleep mode between "power-down" and "idle".
// Copyright (c) 2022 Anton Semjonov
// Licensed under the MIT License

#pragma once

#include <avr/io.h>

// enable power-down sleep mode (only PIT can wake)
#define sleep_configure_powerdown() SLPCTRL.CTRLA = SLPCTRL_SMODE_PDOWN_gc | SLPCTRL_SEN_bm

// enable idle sleep mode (peripherals remain active)
#define sleep_configure_idle() SLPCTRL.CTRLA = SLPCTRL_SMODE_IDLE_gc | SLPCTRL_SEN_bm

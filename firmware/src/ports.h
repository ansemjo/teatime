// Configure the PORTs (input/output pins)
// Copyright (c) 2022 Anton Semjonov
// Licensed under the MIT License

#pragma once

#include <avr/io.h>
#include <avr/interrupt.h>


void disable_unused_pins();
void setup_buttons();
extern void button_add();
extern void button_set();

#define pressed_add   PORTB.IN & PIN6_bm // pressed PB6 (ADD)
#define pressed_set   PORTB.IN & PIN7_bm // pressed PB7 (SET)
#define pressed_both  PORTB.IN & (PIN6_bm | PIN7_bm) // pressed both ADD and SET
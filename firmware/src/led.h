// Configure and control the LED on pin A5
// Copyright (c) 2022 Anton Semjonov
// Licensed under the MIT License

#pragma once

#include <avr/io.h>


#define setup_led()   PORTA.DIRSET = PIN5_bm // configure the LED pin as an output
#define led_toggle()  PORTA.OUTTGL = PIN5_bm // toggle the LED
#define led_on()      PORTA.OUTSET = PIN5_bm // turn the LED on
#define led_off()     PORTA.OUTCLR = PIN5_bm // turn the LED off
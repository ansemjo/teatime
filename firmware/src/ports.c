// Configure the PORTs (input/output pins)
// Copyright (c) 2022 Anton Semjonov
// Licensed under the MIT License

#include "ports.h"


// disable input buffers on unused pins
void disable_unused_pins() {
  // PA1–PA4
  PORTA.PIN1CTRL = PORT_ISC_INPUT_DISABLE_gc;
  PORTA.PIN2CTRL = PORT_ISC_INPUT_DISABLE_gc;
  PORTA.PIN3CTRL = PORT_ISC_INPUT_DISABLE_gc;
  PORTA.PIN4CTRL = PORT_ISC_INPUT_DISABLE_gc;
  // PB4, PB5
  PORTB.PIN4CTRL = PORT_ISC_INPUT_DISABLE_gc;
  PORTB.PIN5CTRL = PORT_ISC_INPUT_DISABLE_gc;
  // PC0–PC5 (complete)
  PORTC.PIN0CTRL = PORT_ISC_INPUT_DISABLE_gc;
  PORTC.PIN1CTRL = PORT_ISC_INPUT_DISABLE_gc;
  PORTC.PIN2CTRL = PORT_ISC_INPUT_DISABLE_gc;
  PORTC.PIN3CTRL = PORT_ISC_INPUT_DISABLE_gc;
  PORTC.PIN4CTRL = PORT_ISC_INPUT_DISABLE_gc;
  PORTC.PIN5CTRL = PORT_ISC_INPUT_DISABLE_gc;
}

// setup buttons with pin-change interrupts
void setup_buttons() {
  // set PB6, PB7 as inputs
  PORTB.DIRCLR = PIN6_bm | PIN7_bm;
  // enable pin interrupts on those pins, no inversion, no pullup
  PORTB.PIN6CTRL = PORT_ISC_BOTHEDGES_gc; // add
  PORTB.PIN7CTRL = PORT_ISC_BOTHEDGES_gc; // set
}

ISR(PORTB_PORT_vect) {
  if (PORTB.INTFLAGS & PORT_INT6_bm) {
    button_add();
    PORTB.INTFLAGS |= PORT_INT6_bm;
  }
  if (PORTB.INTFLAGS & PORT_INT7_bm) {
    button_set();
    PORTB.INTFLAGS |= PORT_INT7_bm;
  }
}
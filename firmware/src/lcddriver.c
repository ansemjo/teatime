// Rohm BU9796AFS / BU9796AMUV commands
// Copyright (c) 2022 Anton Semjonov
// Licensed under the MIT License

#include <avr/io.h>

#include "lcddriver.h"


// configure i2c pins and power on the chip
void setup_lcddriver() {

  // enable i2c pullups
  PORTB.PIN0CTRL = PORT_PULLUPEN_bm;
  PORTB.PIN1CTRL = PORT_PULLUPEN_bm;

  // power on the chip
  PORTA.DIRSET = \
    PIN6_bm  // LCD_VLCD // TODO: use DAC output
  | PIN7_bm; // LCD_VDD
  PORTA.OUTCLR = PIN6_bm; // VLCD low
  PORTA.OUTSET = PIN7_bm; // VDD high

}

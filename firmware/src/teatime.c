// teatime - Simple LCD Timer board
// Copyright (c) 2022 Anton Semjonov
// Licensed under the MIT License

// attiny417 @ 10 mhz
#define F_CPU 10000000L

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>
#include <avr/sleep.h>

#include "lcddriver.h"
#include "segments.h"
#include "i2c_controller.h"

// ---------- system clocks and sleep --------- //

// use internal oscillator for 10 mhz system clock
void setup_system_clock() {
  // make sure OSCCFG.FREQSEL fuse is 0x02 for 20 MHz

  // configure system clock source
  _PROTECTED_WRITE(CLKCTRL.MCLKCTRLA, CLKCTRL_CLKSEL_OSC20M_gc);

  // enable system clock prescaler with 2X division for 10 mhz
  _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, CLKCTRL_PDIV_2X_gc | CLKCTRL_PEN_bm);

  // lock the main clock registers from further modification
  _PROTECTED_WRITE(CLKCTRL.MCLKLOCK, CLKCTRL_LOCKEN_bm);

}

// use external 32.768 khz crystal for rtc
void setup_crystal_rtc() {
  //? Per datasheet: "To change settings in a safe way: write a '0' to
  //? the ENABLE bit and wait until XOSC32KS is '0' before re-enabling
  //? the XOSC32K with new settings."

  // disable oscillator and wait until it's off
  _PROTECTED_WRITE(CLKCTRL.XOSC32KCTRLA, (0 << CLKCTRL_ENABLE_bp));
  while (CLKCTRL.MCLKSTATUS & CLKCTRL_XOSC32KS_bm);

  // enable oscillator on external crystal
  _PROTECTED_WRITE(CLKCTRL.XOSC32KCTRLA, (0 << CLKCTRL_SEL_bp) | CLKCTRL_ENABLE_bm);

  // select crystal as rtc source
  RTC.CLKSEL = RTC_CLKSEL_TOSC32K_gc;

  // enable the periodic interrupt with 1 second period
  RTC.PITCTRLA = RTC_PERIOD_CYC32768_gc | RTC_PITEN_bm;
  while (RTC.STATUS);
  RTC.PITINTCTRL = RTC_PI_bm;

}

// periodic interrupt stub
void periodic_interrupt();
ISR(RTC_PIT_vect) {
  periodic_interrupt();
  RTC.PITINTFLAGS = RTC_PI_bm; // clear the interrupt flag
}

// configure sleep mode
void configure_sleep_powerdown() {
  // enable power-down sleep mode (only PIT can wake)
  SLPCTRL.CTRLA = SLPCTRL_SMODE_PDOWN_gc | SLPCTRL_SEN_bm;
}
void configure_sleep_idle() {
  // enable idle sleep mode (peripherals remain active)
  SLPCTRL.CTRLA = SLPCTRL_SMODE_IDLE_gc | SLPCTRL_SEN_bm;
}


// ---------- external input / output ---------- //

void setup_led() {
  PORTA.DIRSET = PIN5_bm; // LED
}

#define LED_ON()  PORTA.OUTSET = PIN5_bm;
#define LED_OFF() PORTA.OUTCLR = PIN5_bm;
#define LED_TGL() PORTA.OUTTGL = PIN5_bm;

void setup_buttons() {
  PORTB.DIRCLR = PIN6_bm | PIN7_bm; // set PB6, PB7 as inputs
  PORTB.PIN6CTRL = 0; // not inverted, no pullup, no interrupt
  PORTB.PIN7CTRL = 0; // not inverted, no pullup, no interrupt
}

#define BTN_ADD (PORTB.IN & PIN6_bm)
#define BTN_SET (PORTB.IN & PIN7_bm)

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


// ---------- main program ---------- //

uint8_t mybuf[5] = {
  // turn on display from DDRAM
  LCD_MODESET_cmd | LCD_MODESET_ON | LCD_MODESET_bias_03,
  // HELO (reverse order)
  CHAR_0, CHAR_L, CHAR_E, CHAR_H,
  // 0xF5, 0x85, 0x97, 0x67,
};

// display ticks and blink the led in periodic interrupt
volatile uint16_t secs = 0;
void periodic_interrupt() {
  // increment ticks
  secs++;
  // toggle led
  LED_TGL();
  // format display digits from ticks
  mybuf[1] = NUMBERS[(secs     )%10];
  mybuf[2] = NUMBERS[(secs/  10)%10];
  mybuf[3] = NUMBERS[(secs/ 100)%10];
  mybuf[4] = NUMBERS[(secs/1000)%10];
  // add dot on even ticks
  if ((secs % 2) == 0) mybuf[4] |= Ap;
  // write to display controller
  i2c_write(LCD_ADDRESS, mybuf, 5);
}

int main() {

  // setup all the things
  setup_system_clock();
  setup_led();
  setup_crystal_rtc();
  setup_lcddriver();
  setup_buttons();

  configure_sleep_powerdown();

  sei(); // enable interrupts

  i2c_init();
  i2c_write(LCD_ADDRESS, mybuf, 5);

  while (!(TWI0.MSTATUS & TWI_BUSSTATE_IDLE_gc));
  mybuf[0] = 0x00;

  for (;;) sleep_cpu();

}
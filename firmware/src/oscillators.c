// Configure the ATtiny417 system clock and RTC
// Copyright (c) 2022 Anton Semjonov
// Licensed under the MIT License

#include "oscillators.h"


// use internal oscillator for 10 mhz system clock
void setup_10mhz_system_clock() {
  // make sure OSCCFG.FREQSEL fuse is 0x02 for 20 MHz base clock

  // configure system clock source
  _PROTECTED_WRITE(CLKCTRL.MCLKCTRLA, CLKCTRL_CLKSEL_OSC20M_gc);

  // enable system clock prescaler with 2X division for 10 mhz
  _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, CLKCTRL_PDIV_2X_gc | CLKCTRL_PEN_bm);

  // lock the main clock registers from further modification
  _PROTECTED_WRITE(CLKCTRL.MCLKLOCK, CLKCTRL_LOCKEN_bm);

}

// use external 32.768 khz crystal for rtc
void setup_crystal_rtc(RTC_PERIOD_t tick_period) {
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

  // enable the periodic interrupt with with given cycle length
  RTC.PITCTRLA = tick_period | RTC_PITEN_bm;
  while (RTC.STATUS);
  RTC.PITINTCTRL = RTC_PI_bm;

}

// periodic interrupt stub that calls external tick()
ISR(RTC_PIT_vect) {
  tick();
  // clear the interrupt flag
  RTC.PITINTFLAGS = RTC_PI_bm;
}
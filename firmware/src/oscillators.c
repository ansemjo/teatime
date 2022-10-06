// Configure the ATtiny417 system clock and RTC
// Copyright (c) 2022 Anton Semjonov
// Licensed under the MIT License

#include "oscillators.h"


// use internal oscillator for 10 mhz system clock
void setup_system_clock() {
  // make sure OSCCFG.FREQSEL fuse is 0x02 for 20 MHz base clock

  // configure system clock source
  _PROTECTED_WRITE(CLKCTRL.MCLKCTRLA, CLKCTRL_CLKSEL_OSC20M_gc);

  // enable system clock prescaler with 2X division for 10 mhz
  _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, CLKCTRL_PDIV_2X_gc | CLKCTRL_PEN_bm);

  // lock the main clock registers from further modification
  _PROTECTED_WRITE(CLKCTRL.MCLKLOCK, CLKCTRL_LOCKEN_bm);

}

// use external 32.768 khz crystal for rtc
void setup_crystal() {
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

}

// enable the periodic interrupt timer with given period
void setup_pit_ticks(RTC_PERIOD_t tick_period) {

  // enable the periodic interrupt with with given cycle length
  RTC.PITCTRLA = tick_period | RTC_PITEN_bm;
  while (RTC.PITSTATUS);
  RTC.PITINTCTRL = RTC_PI_bm;

}

// enable the rtc timer interrupt to count down seconds
void setup_rtc_seconds() {

  // wait for sync first
  while (RTC.STATUS);

  // run in standby, don't use prescaler
  RTC.CTRLA = RTC_RUNSTDBY_bm | RTC_PRESCALER_DIV1_gc | RTC_RTCEN_bm;

  // configure for an overflow interrupt every second
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    RTC.CNT = 0;
    RTC.PER = 32768;
  }

}

/**
 * Due to the following errata, the RTC should not be disabled via RTC.CTRLA!
 * It will also stop the PIT. Therefore the best we could do is disable the
 * RTC overflow interrupt and save the counter value to restore it later.
 * 
 *  2.6.2 Disabling the RTC Stops the PIT
 *    Writing RTC.CTRLA.RTCEN to ‘0’ will stop the PIT.
 *    Writing RTC.PITCTRLA.PITEN to ‘0’ will stop the RTC.
 *  Work Around
 *    Do not disable the RTC or the PIT if any of the modules are used.
 **/

// start the rtc counter from given count and enable overflow interrupt
void run_rtc(uint16_t cnt) {
  while (RTC.STATUS & RTC_CNTBUSY_bm);
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    RTC.CNT = cnt;
    while (RTC.STATUS & RTC_CNTBUSY_bm);
    RTC.INTCTRL = RTC_OVF_bm;
    RTC.INTFLAGS = RTC_OVF_bm;
  }
}

// disable the rtc overflow interrupt and return current count
uint16_t halt_rtc() {
  uint16_t cnt;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    cnt = RTC.CNT;
    RTC.INTCTRL = (0 << RTC_OVF_bp);
  }
  return cnt;
}

// periodic interrupt stub that calls external tick()
ISR(RTC_PIT_vect) {
  tick();
  // clear the interrupt flag
  RTC.PITINTFLAGS = RTC_PI_bm;
}

// rtc overflow stub that calls external second()
ISR(RTC_CNT_vect) {
  second();
  // clear the interrupt flag
  RTC.INTFLAGS = RTC_OVF_bm;
}
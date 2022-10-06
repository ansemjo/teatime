// teatime - Simple LCD Timer board
// Copyright (c) 2022 Anton Semjonov
// Licensed under the MIT License

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>
#include <avr/sleep.h>

#include "oscillators.h"
#include "sleepmode.h"
#include "lcddriver.h"
#include "segments.h"
#include "lcddriver.h"
#include "i2c_controller.h"
#include "led.h"


// TODO: setup buttons with interrupts
void setup_buttons() {
  PORTB.DIRCLR = PIN6_bm | PIN7_bm; // set PB6, PB7 as inputs
  PORTB.PIN6CTRL = 0; // not inverted, no pullup, no interrupt
  PORTB.PIN7CTRL = 0; // not inverted, no pullup, no interrupt
}
#define BTN_ADD (PORTB.IN & PIN6_bm)
#define BTN_SET (PORTB.IN & PIN7_bm)


uint8_t mybuf[5] = {
  // turn on display from DDRAM
  LCD_MODESET_cmd | LCD_MODESET_ON | LCD_MODESET_bias_03,
  // HELO (reverse order)
  CHAR_0, CHAR_L, CHAR_E, CHAR_H,
  // 0xF5, 0x85, 0x97, 0x67,
};

// display ticks and blink the led in periodic interrupt
volatile uint16_t secs = 0;
void tick() {
  // increment ticks
  secs++; led_toggle();
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
  setup_10mhz_system_clock();
  setup_crystal_rtc(RTC_PERIOD_CYC512_gc);
  setup_led();
  setup_lcddriver();
  setup_buttons();
  sleep_configure_powerdown();
  sei(); // enable interrupts

  i2c_init();
  i2c_write(LCD_ADDRESS, mybuf, 5);

  while (!(TWI0.MSTATUS & TWI_BUSSTATE_IDLE_gc));
  mybuf[0] = 0x00;

  for (;;) sleep_cpu();

}
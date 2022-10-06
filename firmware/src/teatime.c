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

  // set PB6, PB7 as inputs
  PORTB.DIRCLR = PIN6_bm | PIN7_bm;
  // enable pin interrupts on those pins, no inversion, no pullup
  PORTB.PIN6CTRL = PORT_ISC_BOTHEDGES_gc;
  PORTB.PIN7CTRL = PORT_ISC_BOTHEDGES_gc;

}
#define BTN_ADD (PORTB.IN & PIN6_bm)
#define BTN_SET (PORTB.IN & PIN7_bm)

volatile int8_t btn_add = 0;
volatile int8_t btn_set = 0;

ISR(PORTB_PORT_vect) {
  // ADD
  if (PORTB.INTFLAGS & PORT_INT6_bm) {
    if (PORTB.IN & PIN6_bm) btn_add++; else btn_add--;
    PORTB.INTFLAGS |= PORT_INT6_bm;
  }
  // SET
  if (PORTB.INTFLAGS & PORT_INT7_bm) {
    if (PORTB.IN & PIN7_bm) btn_set++; else btn_set--;
    PORTB.INTFLAGS |= PORT_INT7_bm;
  }
}

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

uint8_t mybuf[5] = {
  // turn on display from DDRAM
  LCD_MODESET_cmd | LCD_MODESET_ON | LCD_MODESET_bias_03,
  // HELO (reverse order)
  CHAR_0, CHAR_L, CHAR_E, CHAR_H,
  // 0xF5, 0x85, 0x97, 0x67,
};

// display ticks and blink the led in periodic interrupt
volatile uint16_t secs = 0;
void tick_old() {
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

// display button interrupt counts
void tick() {
  if (btn_set < 0) {
    mybuf[1] = NUMBERS[btn_set*-1];
    mybuf[2] = CHAR_MINUS;
  } else {
    mybuf[1] = NUMBERS[btn_set%10];
    mybuf[2] = CHAR_SPACE;
  }
  if (btn_add < 0) {
    mybuf[3] = NUMBERS[btn_add*-1];
    mybuf[4] = CHAR_MINUS;
  } else {
    mybuf[3] = NUMBERS[btn_add%10];
    mybuf[4] = CHAR_SPACE;
  }
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
  disable_unused_pins();
  sei(); // enable interrupts

  i2c_init();
  i2c_write(LCD_ADDRESS, mybuf, 5);

  while (!(TWI0.MSTATUS & TWI_BUSSTATE_IDLE_gc));
  mybuf[0] = 0x00;

  for (;;) sleep_cpu();

}
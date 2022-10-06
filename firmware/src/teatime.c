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
#include "ports.h"
#include "led.h"

// counters for button press duration
volatile uint8_t btn_add_count = 0;
volatile bool    btn_add_was_long = false;
volatile uint8_t btn_set_count = 0;
volatile bool    btn_set_was_long = false;

// how many ticks for a long press
const int long_len = 32;

// "state machine"
typedef enum {
  idle = 0,
  running,
  paused,
  finished,
} teatime_state;
volatile teatime_state state = idle;

// mini buffer for lcd commands
uint8_t lcd_commands[3] = { 0x00, 0x00, 0x00 };

// set countdown value
volatile uint16_t countdown_preset = 0;
volatile uint16_t countdown = 0;
volatile uint16_t rtc_value = 0;

/**
 * State machine with button presses:
 * (×btn = short press, |btn = long press)
 * 
 * [0] "00:00" set time
 *    ×add --> + 10 seconds
 *    |add --> + 1 minute (+ hold)
 *    ×set --> [1] running, store countdown value as preset
 * 
 * [1] running, time runs down, switch to [2] end on zero
 *    ×add, |add --> as [0], but not added to preset
 *    ×set --> pause/unpause
 * 
 * [2] end, led and display is blinking
 *    ×set --> reset to [0] with previously preset time
 * 
**/

void button_add() {
  if (!(pressed_add)) {
    // long press handled in tick()
    if (state != finished) {
      if (!btn_add_was_long) countdown += 10;
      if (countdown >= 6000) countdown = 5999;
    }
    btn_add_count = 0;
    btn_add_was_long = false;
  }
}

void button_set() {
  if (pressed_set) {

    switch (state) {
      case idle:
        if (countdown == 0) {
          led_toggle();
          break;
        }
        countdown_preset = countdown;
        run_rtc(1);
        state = running;
        break;

      case running:
        rtc_value = halt_rtc();
        lcd_commands[0] = LCD_BLKCTL_cmd | LCD_BLKCTL_1Hz;
        i2c_write(LCD_ADDRESS, lcd_commands, 1);
        state = paused;
        break;

      case paused:
        run_rtc(rtc_value);
        lcd_commands[0] = LCD_BLKCTL_cmd | LCD_BLKCTL_off;
        i2c_write(LCD_ADDRESS, lcd_commands, 1);
        state = running;
        break;

      case finished:
        countdown = countdown_preset;
        lcd_commands[0] = LCD_BLKCTL_cmd | LCD_BLKCTL_off;
        i2c_write(LCD_ADDRESS, lcd_commands, 1);
        led_off();
        state = idle;
        break;
      
      default:
        break;
    }
  } else {
    if (state == idle) {
      led_off();
    }
    btn_set_count = 0;
    btn_set_was_long = false;
  }
}

uint8_t mybuf[5] = {
  // turn on display from DDRAM
  LCD_MODESET_cmd | LCD_MODESET_ON | LCD_MODESET_bias_03,
  // HELO (reverse order)
  CHAR_0, CHAR_L, CHAR_E, CHAR_H,
  // 0xF5, 0x85, 0x97, 0x67,
};

// display the countdown time in 12:34 format
void display_time(uint16_t time) {
  // split into mins:secs
  uint8_t secs = time % 60;
  uint8_t mins = time / 60;
  // format seconds
  mybuf[1] = NUMBERS[ secs       % 10];
  mybuf[2] = NUMBERS[(secs / 10) % 10];
  mybuf[3] = NUMBERS[ mins       % 10] | Ap;
  mybuf[4] = NUMBERS[(mins / 10) % 10];
  // when running, blink the colon
  if ((state == running) && (secs % 2) == 1) {
    mybuf[3] &= ~Ap;
  }
  i2c_write(LCD_ADDRESS, mybuf, 5);
}


// display button interrupt counts
void tick() {
  if (pressed_add && state != finished) {
    btn_add_count++;
    if (btn_add_count >= long_len) {
      btn_add_was_long = true;
      countdown += 60;
      if (countdown >= 6000) countdown = 5999;
      btn_add_count = 0;
    }
  }
  if (pressed_set) {
    if (btn_set_count < long_len*2) {
      btn_set_count++;
    } else if (!btn_set_was_long) {
      btn_set_was_long = true;
      halt_rtc();
      countdown_preset = countdown = 0;
      lcd_commands[0] = LCD_BLKCTL_cmd | LCD_BLKCTL_off;
      i2c_write(LCD_ADDRESS, lcd_commands, 1);
      led_off();
      state = idle;
      btn_set_count = 0;
    }
  }
  display_time(countdown);
}

// count down
void second() {
  if (countdown > 0)
    countdown--;
  if (countdown == 0) {
    halt_rtc();
    state = finished;
    lcd_commands[0] = LCD_BLKCTL_cmd | LCD_BLKCTL_2Hz;
    i2c_write(LCD_ADDRESS, lcd_commands, 1);
    led_on();
  }
}

int main() {

  // setup all the things
  setup_system_clock();
  setup_crystal();
  setup_pit_ticks(RTC_PERIOD_CYC512_gc);
  setup_rtc_seconds();
  setup_led();
  setup_lcddriver();
  setup_buttons();
  sleep_configure_standby();
  sleep_enable();
  disable_unused_pins();
  sei(); // enable interrupts

  i2c_init();
  i2c_write(LCD_ADDRESS, mybuf, 5);

  i2c_wait_until_idle();
  mybuf[0] = 0x00;

  for (;;) sleep_cpu();

}
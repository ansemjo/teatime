// Rohm BU9796AFS / BU9796AMUV commands
// Copyright (c) 2022 Anton Semjonov
// Licensed under the MIT License

#pragma once

/**
 * A few example commands to drive the controller directly.
 * (Either put 0Ω resistors in R6 R7 or just set A6 LOW and A7 HIGH.)
 *
 * Reset the controller:
 * $ i2ctransfer -y 12 w1@0x3E 0x6A # 0b01101010
 *
 * Enable outputs (start displaying from DDRAM):
 * $ i2ctransfer -y 12 w1@0x3E 0x48 # 0b01001000
 *
 * Set contents to "HELO":
 * $ i2ctransfer -y 12 w5@0x3E 0x00 0xF5 0x85 0x97 0x67
 *
 * Override; all segments on:
 * $ i2ctransfer -y 12 w1@0x3E 0x7E # 0b01111110
 *
 **/

// lcd driver's i2c address
#define LCD_ADDRESS 0x3E

// there's another command in the next byte
#define CMDBIT 0b10000000

#define LCD_DISCTL_cmd 0b00100000 // display control command
#define LCD_DISCTL_framerate_80Hz 0b00 << 3 // normal mode, 80Hz framerate
#define LCD_DISCTL_framerate_71Hz 0b01 << 3 // power save 1, 71Hz framerate
#define LCD_DISCTL_framerate_64Hz 0b10 << 3 // power save 2, 64Hz framerate
#define LCD_DISCTL_framerate_50Hz 0b11 << 3 // power save 3, 50Hz framerate
#define LCD_DISCTL_waveform_lineinv  0b0 << 2 // line inversion drive waveform
#define LCD_DISCTL_waveform_frameinv 0b1 << 2 // frame inversion drive waveform
#define LCD_DISCTL_powersave_psm1 0b00 << 0 // power save mode 1 (x0.5)
#define LCD_DISCTL_powersave_psm2 0b01 << 0 // power save mode 2 (x0.67)
#define LCD_DISCTL_powersave_norm 0b10 << 0 // normal mode (x1.0)
#define LCD_DISCTL_powersave_high 0b11 << 0 // high power mode (x1.8)

#define LCD_MODESET_cmd 0b01000000 // display mode setting
#define LCD_MODESET_OFF 0b0 << 3 // display OFF
#define LCD_MODESET_ON  0b1 << 3 // display ON
#define LCD_MODESET_bias_03 0b0 << 2 // 1/3 bias
#define LCD_MODESET_bias_05 0b1 << 2 // 1/2 bias

#define LCD_ADSET_mask 0b00011111 // address setting mask before data write
#define LCD_ADSET_max  0b00010011 // maximum allowed address (19)

#define LCD_ICSET_cmd 0b01101000 // chip operation commands
#define LCD_ICSET_reset 0b1 << 1 // execute software reset
#define LCD_ICSET_osc_int 0b0 << 0 // use internal oscillator
#define LCD_ICSET_osc_ext 0b1 << 0 // use external clock input

#define LCD_BLKCTL_cmd 0b01110000 // blink control command
#define LCD_BLKCTL_off  0b00 // off, steady output
#define LCD_BLKCTL_05Hz 0b01 // blink with 0.5Hz
#define LCD_BLKCTL_1Hz  0b10 // blink with 1Hz
#define LCD_BLKCTL_2Hz  0b11 // blink with 2Hz

#define LCD_APCTL_cmd 0b01111100 // all pixel control command
#define LCD_APCTL_APON_normal  0b0 << 1 // do not force all pixels on
#define LCD_APCTL_APON_allON   0b1 << 1 // force all pixels ON
#define LCD_APCTL_APOFF_normal 0b0 << 0 // do not force all pixels off
#define LCD_APCTL_APOFF_allOFF 0b1 << 0 // force all pixels OFF


void setup_lcddriver();
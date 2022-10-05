// Use 1-series TWI for I2C controller communications.
// Copyright (c) 2022 Anton Semjonov
// Licensed under the MIT License

#pragma once

#include <stdint.h>
#include <stdbool.h>

// use fast mode with 400 khz
#define F_SCL 400000L // [hz]

// rise time from datasheet, table 34-19 (t_R)
#define T_RISE 300L // [ns]

// calculate controller baud rate
//      f_scl = f_cpu / (10 + 2·BAUD + f_cpu·t_rise)
//  --> 10 + 2·BAUD + f_cpu·t_rise = f_cpu/f_scl
//  --> BAUD = ( f_cpu/f_scl - f_cpu·t_rise - 10 ) / 2
#define TWI_BAUD ( (F_CPU/F_SCL) - (F_CPU*T_RISE/1000000000) - 10 ) / 2


// bitmap of status register // TODO
#define I2C_IN_PROGRESS (1 << 0) // a transaction has been started in software
#define I2C_START_OK    (1 << 1) // start condition sent and address was acknowleged
#define I2C_BUS_ERROR   (1 << 2) // either arbitration was lost or another bus error
#define I2C_NACKED      (1 << 3) // device sent back a NACK, either unsuccessful write or no more read

typedef enum {
  success = 0,
  in_progress,
  arbitration_lost,
  address_nack,
} i2c_error;

i2c_error i2c_result;

void i2c_init();
bool i2c_write(uint8_t address, const uint8_t *buf, const uint8_t len);

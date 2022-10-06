// Use 1-series TWI for I2C controller communications.
// Copyright (c) 2022 Anton Semjonov
// Licensed under the MIT License

#include "i2c_controller.h"
#include "sleepmode.h"


// ---------- init and basics ---------- //

// TODO: use a single bitfield "register"?
i2c_error i2c_result = success;

// hold current and end pointers
static volatile uint8_t *buf = NULL;
static volatile uint8_t *end = NULL;


// configure the twi controller for interrupt-driven operation
void i2c_init() {

  #if F_SCL == 1000000L
  // enable fast mode plus (Fm+)
  TWI0.CTRLA = TWI_FMPEN_bm;
  #endif

  // set the calculated baudrate
  TWI0.MBAUD = TWI_BAUD;

  // enable peripheral in "master" mode
  TWI0.MCTRLA = \
    TWI_RIEN_bm | TWI_WIEN_bm   // enable interrupt vector
  | TWI_TIMEOUT_DISABLED_gc     // no timeout for i2c
  | TWI_ENABLE_bm;

  // force the bus state into IDLE
  TWI0.MSTATUS = TWI_BUSSTATE_IDLE_gc;

}

// begin a transaction by writing an address with direction bit
void i2c_start(uint8_t address, bool reading) {

  // write the address
  TWI0.MADDR = address << 1 | (reading ? 1 : 0);

  // configure idle sleep mode (peripherals remain active)
  sleep_configure_idle();

}

// clear buffers and end a transaction "in software"
void i2c_end(i2c_error err) {

  // store transaction result and clear pointers
  i2c_result = err;
  buf = end = NULL;

  // enable power-down sleep mode (only PIT can wake)
  sleep_configure_powerdown();

}

// stop a transaction by generating a stop condition
void i2c_stop() {

  // generate the stop condition on the bus and wait for idle
  TWI0.MCTRLB |= TWI_MCMD_STOP_gc;
  // TODO: we don't want busy waiting in an ISR, do we?
  while (!(TWI0.MSTATUS & TWI_BUSSTATE_IDLE_gc));

}


// ---------- writing ---------- //

bool i2c_write(uint8_t address, const uint8_t *data, const uint8_t length) {

  // abort if another transmission is already running
  if (buf != NULL) return false;

  // also abort if bus state is not currently idle
  if (!(TWI0.MSTATUS & TWI_BUSSTATE_IDLE_gc)) return false;

  // otherwise start a transmission, store pointer and length
  buf = data;
  end = data + length;

  // write address and configure sleep mode
  i2c_start(address, false);

}

// ---------- interrupt handler ---------- //

ISR(TWI0_TWIM_vect) {

  // error: arbitration lost or bus error
  if (TWI0.MSTATUS & (TWI_ARBLOST_bm | TWI_BUSERR_bm)) {
    i2c_end(arbitration_lost);
  }

  // error: no such address
  if (TWI0.MSTATUS & TWI_RXACK_bm) {
    i2c_stop();
    i2c_end(address_nack);
  }

  // error: pointers are not set ?!
  if ((buf == NULL) || (end == NULL)) {
    return; // ?!
  }

  // WIF --> write interrupt
  if (TWI0.MSTATUS & TWI_WIF_bm) {

    // if there is data remaining ...
    if (buf < end) {

      // write next byte
      TWI0.MCTRLB = TWI_ACKACT_ACK_gc | TWI_MCMD_RECVTRANS_gc;
      TWI0.MDATA = *buf++;

    } else {

      // end the transaction
      i2c_stop();
      i2c_end(success);

    }
  }

  // RIF --> read interrupt // TODO: untested
  if (TWI0.MSTATUS & TWI_RIF_bm) {

    // if more data is to be read ...
    if (buf < end) {
      // copy the last read byte
      *buf++ = TWI0.MDATA;
    }

    // if we want to read another byte ...
    // note: this is *after* incrementing buf above
    if (buf < end) {

      // send an ACK
      TWI0.MCTRLB = TWI_ACKACT_ACK_gc | TWI_MCMD_RECVTRANS_gc;

    } else { // or if this was the last byte we want to read ...

      // send a NACK and stop
      TWI0.MCTRLB = TWI_ACKACT_NACK_gc;
      i2c_stop();
      i2c_end(success);

    }

  }

}



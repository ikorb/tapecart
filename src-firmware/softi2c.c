/* tapecart - a tape port storage pod for the C64

   Copyright (C) 2013-2017  Ingo Korb <ingo@akana.de>
   All rights reserved.
   Idea by enthusi

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
   FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
   OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
   OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
   SUCH DAMAGE.


   softi2c.c: Software I2C bus master

*/

#include <avr/io.h>
#include "config.h"
#include "timer.h"
#include "i2c.h"

#define SOFTI2C_SDA _BV(SOFTI2C_BIT_SDA)
#define SOFTI2C_SCL _BV(SOFTI2C_BIT_SCL)

static void set_scl(uint8_t x) {
  if (x) {
    SOFTI2C_DDR  &= (uint8_t)~SOFTI2C_SCL;
    SOFTI2C_PORT |= SOFTI2C_SCL;
    // Clock stretching
    loop_until_bit_is_set(SOFTI2C_PIN, SOFTI2C_BIT_SCL);
  } else {
    SOFTI2C_DDR  |= SOFTI2C_SCL;
    SOFTI2C_PORT &= (uint8_t)~SOFTI2C_SCL;
  }
}

static void set_sda(uint8_t x) {
  if (x) {
    SOFTI2C_DDR  &= (uint8_t)~SOFTI2C_SDA;
    SOFTI2C_PORT |= SOFTI2C_SDA;
  } else {
    SOFTI2C_DDR  |= SOFTI2C_SDA;
    SOFTI2C_PORT &= (uint8_t)~SOFTI2C_SDA;
  }
}


void i2c_start(void) {
  set_sda(1);
  set_scl(1);
  delay_us(SOFTI2C_DELAY);
  set_sda(0);
  delay_us(SOFTI2C_DELAY);
  set_scl(0);
}

void i2c_stop(void) {
  set_sda(0);
  delay_us(SOFTI2C_DELAY);
  set_scl(1);
  delay_us(SOFTI2C_DELAY);
  set_sda(1);
  delay_us(SOFTI2C_DELAY);
}

/* Returns value of the acknowledge bit */
uint8_t i2c_send_byte(uint8_t value) {
  uint8_t i;

  for (i=8;i!=0;i--) {
    set_scl(0);
    delay_us(SOFTI2C_DELAY/2);
    set_sda(value & 128);
    delay_us(SOFTI2C_DELAY/2);
    set_scl(1);
    delay_us(SOFTI2C_DELAY);
    value <<= 1;
  }
  set_scl(0);
  delay_us(SOFTI2C_DELAY/2);
  set_sda(1);
  delay_us(SOFTI2C_DELAY/2);
  set_scl(1);
  delay_us(SOFTI2C_DELAY/2);
  i = !!(SOFTI2C_PIN & SOFTI2C_SDA);
  delay_us(SOFTI2C_DELAY/2);
  set_scl(0);

  return i;
}

/* Returns value of the byte read */
uint8_t i2c_recv_byte(uint8_t sendack) {
  uint8_t i;
  uint8_t value = 0;

  set_sda(1);
  delay_us(SOFTI2C_DELAY/2);
  for (i=8;i!=0;i--) {
    delay_us(SOFTI2C_DELAY/2);
    set_scl(1);
    delay_us(SOFTI2C_DELAY/2);
    value = (value << 1) + !!(SOFTI2C_PIN & SOFTI2C_SDA);
    delay_us(SOFTI2C_DELAY/2);
    set_scl(0);
    delay_us(SOFTI2C_DELAY/2);
  }
  set_sda(!sendack);
  delay_us(SOFTI2C_DELAY/2);
  set_scl(1);
  delay_us(SOFTI2C_DELAY);
  set_scl(0);
  set_sda(1);

  return value;
}

void i2c_init(void) {
  /* Set I2C pins to input -> high with external pullups */
  SOFTI2C_DDR  &= (uint8_t)~(SOFTI2C_SCL|SOFTI2C_SDA);
  SOFTI2C_PORT &= (uint8_t)~(SOFTI2C_SCL|SOFTI2C_SDA);

  set_sda(1);
  set_scl(1);

  /* generate 9 pulses on SCL */
  for (uint8_t i = 0; i < 9; i++) {
    delay_us(SOFTI2C_DELAY);
    set_scl(0);
    delay_us(SOFTI2C_DELAY);
    set_scl(1);
  }

  /* send a stop condition */
  i2c_stop();
}

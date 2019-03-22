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


   tapecartif.c: Interface to the tapecart

*/

#include <6502.h>
#include <c64.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <peekpoke.h>
#include "minidelay.h"
#include "tapecartif.h"


#define CPUPORT_nMOTOR (1 << 5)
#define CPUPORT_SENSE  (1 << 4)
#define CPUPORT_WRITE  (1 << 3)

#define TAPECART_CMDMODE_MAGIC 0xfce2

#define CPUDDR  (*(unsigned char *) 0)
#define CPUPORT (*(unsigned char *) 1)

#define CIA_ICR_FLAG (1 << 4)

#define MOTOR_FLAG (*(unsigned char *) 0xc0)

#define SENSE_LOOP_TIMEOUT 30000
#define PULSE_LOOP_TIMEOUT 10000

/* note: "state" is the state of the tape port line, not the CPU bit */
static void cpuport_motor_on(void) {
  CPUPORT &= ~CPUPORT_nMOTOR;
}

static void cpuport_motor_off(void) {
  CPUPORT |= CPUPORT_nMOTOR;
}

static void cpuport_write_low(void) {
  CPUPORT &= ~CPUPORT_WRITE;
}

static void cpuport_write_high(void) {
  CPUPORT |= CPUPORT_WRITE;
}

static void cpuport_write(const bool state) {
  if (state)
    cpuport_write_high();
  else
    cpuport_write_low();
}

static void cpuport_sense(const bool state) {
  if (state)
    CPUPORT |=  CPUPORT_SENSE;
  else
    CPUPORT &= ~CPUPORT_SENSE;
}

static uint8_t dummy_variable;

/* assumed initial state: write low */
void tapecart_sendbyte(uint8_t byte) {
  uint8_t i;

  SEI();

  /* wait until tapecart is ready (sense high) */
  while (!(CPUPORT & CPUPORT_SENSE)) ;

  /* switch sense to output */
  CPUDDR = 0x2f | CPUPORT_SENSE;

  /* send byte */
  for (i = 0; i < 8; ++i) {
    cpuport_sense(byte & 0x80);

    cpuport_write_high();
    cpuport_write_low();

    byte <<= 1;
  }

  /* ensure known level on sense */
  cpuport_sense(true);

  /* switch sense to input */
  /* (tapecart sets sense to output 10us after high->low edge) */
  cpuport_sense(false);
  CPUDDR = 0x2f;
  tinydelay();

  /* make sure that the kernal does not turn on the motor */
  MOTOR_FLAG = 1;

  CLI();
}

/* assumed initial state: write output low, sense input */
uint8_t tapecart_getbyte(void) {
  uint8_t byte, i;

  SEI();

  /* wait until tapecart is ready (sense high) */
  while (!(CPUPORT & CPUPORT_SENSE)) ;

  /* set write high to start the transmission on the tapecart side */
  cpuport_write_high();
  tinydelay();

  /* read byte */
  for (i = 0; i < 8; ++i) {
    cpuport_write_low();
    cpuport_write_high();

    byte = byte << 1;
    if (CPUPORT & CPUPORT_SENSE)
      byte |= 1;
  }

  /* return write to low */
  cpuport_write_low();

  /* make sure that the kernal does not turn on the motor */
  MOTOR_FLAG = 1;

  CLI();

  return byte;
}

void tapecart_send_u24(uint32_t value) {
  tapecart_sendbyte( value & 0xff);
  tapecart_sendbyte((value >> 8) & 0xff);
  tapecart_sendbyte((value >> 16) & 0xff);
}

void tapecart_send_u16(uint16_t value) {
  tapecart_sendbyte(value & 0xff);
  tapecart_sendbyte(value >> 8);
}

uint32_t tapecart_get_u24(void) {
  uint32_t val;

  val  = tapecart_getbyte();
  val |= (uint32_t)tapecart_getbyte() << 8;
  val |= (uint32_t)tapecart_getbyte() << 16;

  return val;
}

uint16_t tapecart_get_u16(void) {
  uint16_t val;

  val  = tapecart_getbyte();
  val |= (uint16_t)tapecart_getbyte() << 8;

  return val;
}

bool tapecart_cmdmode(void) {
  uint16_t cmdcode = TAPECART_CMDMODE_MAGIC;
  unsigned char i;
  unsigned int timeout;

  /* ensure the cart is in stream mode */
  cpuport_write_low();
  SEI();
  cpuport_motor_on();
  minidelay();

  /* wait until sense is low */
  timeout = SENSE_LOOP_TIMEOUT;
  while (timeout > 0 && (CPUPORT & CPUPORT_SENSE))
    --timeout;

  if (timeout == 0) {
    CLI();
    return false;
  }

  /* send unlock sequence - needs ~70 ms */
  cpuport_motor_off();
  minidelay();

  for (i = 0; i < 16; ++i) {
    cpuport_write(!!(cmdcode & 0x8000));
    cmdcode <<= 1;

    minidelay();
    cpuport_motor_on();

    minidelay();
    cpuport_motor_off();
  }

  minidelay();
  cpuport_write_high();

  /* wait until sense is high */
  timeout = SENSE_LOOP_TIMEOUT;
  while (timeout > 0 && !(CPUPORT & CPUPORT_SENSE))
    --timeout;
  if (timeout == 0) {
    CLI();
    return false;
  }

  /* wait for 100 pulses on read */
  minidelay();
  for (i = 0; i < 100; ++i) {
    dummy_variable = CIA1.icr;
    timeout = PULSE_LOOP_TIMEOUT;
    while (timeout > 0 && !(CIA1.icr & CIA_ICR_FLAG))
      --timeout;

    if (timeout == 0) {
      CLI();
      return false;
    }
  }

  cpuport_write_low();

  /* make sure that the kernal does not turn on the motor */
  MOTOR_FLAG = 1;

  CLI();

  return true;
}

void tapecart_streammode(void) {
  /* return to stream mode by turning on the motor line */
  SEI();
  cpuport_motor_on();
  minidelay();
  CLI();
}

void tapecart_read_loader(uint8_t *data) {
  uint8_t i;

  tapecart_sendbyte(CMD_READ_LOADER);
  for (i = 0; i < LOADER_LENGTH; ++i) {
    *data++ = tapecart_getbyte();
  }
}

void tapecart_open_dir(char *path, char *new_path) {
  uint8_t  i;

  tapecart_sendbyte(CMD_SD_OPEN_DIR);
  for (i = 0; i < FILENAME_LENGTH; ++i) {
    tapecart_sendbyte(*path++);
  }

  for (i = 0; i < FILENAME_LENGTH; ++i) {
    *new_path++ = tapecart_getbyte();
  }
}

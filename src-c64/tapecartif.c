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

/* note: "state" is the state of the tape port line, not the CPU bit */
static void cpuport_motor_on(void) {
  CPUPORT &= ~CPUPORT_nMOTOR;
}

static void cpuport_motor_off(void) {
  CPUPORT |= CPUPORT_nMOTOR;
}

/* unused
static void cpuport_motor(const bool state) {
  if (state)
    cpuport_motor_on();
  else
    cpuport_motor_off();
}
*/

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

void tapecart_cmdmode(void) {
  uint16_t cmdcode = TAPECART_CMDMODE_MAGIC;
  unsigned char i;

  /* ensure the cart is in stream mode */
  cpuport_write_low();
  SEI();
  cpuport_motor_on();
  minidelay();

  /* wait until sense is low */
  while (CPUPORT & CPUPORT_SENSE) ;

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
  while (!(CPUPORT & CPUPORT_SENSE)) ;

  /* wait for 100 pulses on read */
  minidelay();
  for (i = 0; i < 100; ++i) {
    dummy_variable = CIA1.icr;
    while (!(CIA1.icr & CIA_ICR_FLAG)) ;
  }

  cpuport_write_low();

  /* make sure that the kernal does not turn on the motor */
  MOTOR_FLAG = 1;

  CLI();
}

void tapecart_streammode(void) {
  /* return to stream mode by turning on the motor line */
  SEI();
  cpuport_motor_on();
  minidelay();
  CLI();
}

void tapecart_read_flash(uint32_t offset, uint16_t len, void *data) {
  uint16_t i;
  uint8_t *bytedata = (uint8_t *)data;

  tapecart_sendbyte(CMD_READ_FLASH);
  tapecart_send_u24(offset);
  tapecart_send_u16(len);

  for (i = 0; i < len; ++i) {
    *bytedata++ = tapecart_getbyte();
  }
}

void tapecart_read_flash_fast(uint32_t offset, uint16_t len, void *data) {
  uint16_t i;
  uint8_t *bytedata = (uint8_t *)data;

  tapecart_sendbyte(CMD_READ_FLASH_FAST);
  tapecart_send_u24(offset);
  tapecart_send_u16(len);

  SEI();
  for (i = 0; i < len; ++i) {
    *bytedata++ = tapecart_getbyte_fast();
  }
  CLI();
}

void tapecart_write_flash(uint32_t offset, uint16_t len, const void *data) {
  uint16_t i;
  const uint8_t *bytedata = (const uint8_t *)data;

  tapecart_sendbyte(CMD_WRITE_FLASH);
  tapecart_send_u24(offset);
  tapecart_send_u16(len);

  for (i = 0; i < len; ++i) {
    tapecart_sendbyte(*bytedata++);
  }
}

void tapecart_erase_flashblock(uint32_t offset) {
  tapecart_sendbyte(CMD_ERASE_FLASH_BLOCK);
  tapecart_send_u24(offset);
}

void tapecart_erase_64k(uint32_t offset) {
  tapecart_sendbyte(CMD_ERASE_FLASH_64K);
  tapecart_send_u24(offset);
}

void tapecart_read_loader(uint8_t *data) {
  uint8_t i;

  tapecart_sendbyte(CMD_READ_LOADER);
  for (i = 0; i < LOADER_LENGTH; ++i) {
    *data++ = tapecart_getbyte();
  }
}

void tapecart_write_loader(const uint8_t *data) {
  uint8_t i;

  tapecart_sendbyte(CMD_WRITE_LOADER);
  for (i = 0; i < LOADER_LENGTH; ++i) {
    tapecart_sendbyte(data[i]);
  }
}

void tapecart_read_loadinfo(uint16_t *offset, uint16_t *length, uint16_t *calladdr, char *filename) {
  uint8_t  i;
  
  tapecart_sendbyte(CMD_READ_LOADINFO);

  *offset   = tapecart_get_u16();
  *length   = tapecart_get_u16();
  *calladdr = tapecart_get_u16();

  for (i = 0; i < FILENAME_LENGTH; ++i) {
    *filename++ = tapecart_getbyte();
  }
}

void tapecart_write_loadinfo(const uint16_t offset,
                             const uint16_t length,
                             const uint16_t calladdr,
                             const char    *filename) {
  uint8_t i;

  tapecart_sendbyte(CMD_WRITE_LOADINFO);

  tapecart_send_u16(offset);
  tapecart_send_u16(length);
  tapecart_send_u16(calladdr);

  for (i = 0; i < FILENAME_LENGTH; ++i) {
    tapecart_sendbyte(*filename++);
  }
}

void tapecart_read_sizes(uint32_t *total_size, uint16_t *page_size, uint16_t *erase_pages) {
  tapecart_sendbyte(CMD_READ_DEVICESIZES);

  *total_size  = tapecart_get_u24();
  *page_size   = tapecart_get_u16();
  *erase_pages = tapecart_get_u16();
}

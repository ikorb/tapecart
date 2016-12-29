/* tapecart - a tape port storage pod for the C64

   Copyright (C) 2013-2016  Ingo Korb <ingo@akana.de>
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


   extmem_at45.c: AT45DB flash chip external memory

*/

#include "config.h"
#include "bitbanging.h"
#include "spi.h"
#include "timer.h"
#include "watchdog.h"
#include "extmem.h"

#define ERASE_MASK ((CONFIG_EXTMEM_SIZE - 1) & ~(CONFIG_EXTMEM_PAGE_SIZE - 1))

#define CMD_READ_CONT_LOW      0x03
#define CMD_MEM_TO_BUFFER_1    0x53
#define CMD_PAGE_ERASE         0x81
#define CMD_SECTOR_ERASE       0x7c
#define CMD_BUFFER_ERASE_PGM_1 0x83
#define CMD_BUFFER_WRITE_1     0x84
#define CMD_READ_STATUS        0xd7
#define CMD_READ_DEVICE_ID     0x9f

#define STATUS_PAGESIZE        (1<<0)
#define STATUS_READY           (1<<7)

static uint24 current_write_addr;

static void spi_send_24bit(uint24 address) {
  spi_exchange_byte((address >> 16) & 0xff);
  spi_exchange_byte((address >>  8) & 0xff);
  spi_exchange_byte( address        & 0xff);
}

void extmem_init(void) {
  spi_init();

  /* check if the flash is already set to 512 byte page size */
  spi_set_ss(SPISS_LOW);
  spi_exchange_byte(CMD_READ_STATUS);
  uint8_t res = spi_exchange_byte(0xff);
  spi_set_ss(SPISS_HIGH);

  if (!(res & STATUS_PAGESIZE)) {
    set_led(true);

    /* switch device to 512 byte pages */
    spi_set_ss(SPISS_LOW);
    spi_exchange_byte(0x3d);
    spi_exchange_byte(0x2a);
    spi_exchange_byte(0x80);
    spi_exchange_byte(0xa6);
    spi_set_ss(SPISS_HIGH);

    /* wait until device is no longer busy */
    while (extmem_write_isbusy()) ;

    /* flicker LED */
    while (1) {
      set_led(true);
      delay_ms(50);
      set_led(false);
      delay_ms(50);
    }
  }
}

void extmem_read_start(uint24 address) {
  spi_set_ss(SPISS_LOW);
  spi_exchange_byte(CMD_READ_CONT_LOW);
  spi_send_24bit(address);
}

uint8_t extmem_read_byte(bool last_byte) {
  return spi_exchange_byte(0xff);
}

void extmem_read_stop(void) {
  spi_set_ss(SPISS_HIGH);
}

void extmem_write_start(uint24 address) {
  // FIXME: Read current page contents into buffer to allow partial writes
  spi_set_ss(SPISS_LOW);
  spi_exchange_byte(CMD_BUFFER_WRITE_1);
  spi_exchange_byte(0);
  spi_exchange_byte(0);
  spi_exchange_byte(0);

  current_write_addr = address;
}

void extmem_write_byte(uint8_t byte) {
  spi_exchange_byte(byte);
}

void extmem_write_stop(void) {
  /* stop writing to the internal buffer */
  spi_set_ss(SPISS_HIGH);

  /* now write the buffer into the flash array */
  spi_set_ss(SPISS_LOW);
  spi_exchange_byte(CMD_BUFFER_ERASE_PGM_1);
  spi_send_24bit(current_write_addr);
  spi_set_ss(SPISS_HIGH);
}

bool extmem_write_isbusy(void) {
  uint8_t result;

  spi_set_ss(SPISS_LOW);
  spi_exchange_byte(CMD_READ_STATUS);
  result = spi_exchange_byte(0xff);
  spi_set_ss(SPISS_HIGH);

  return !(result & STATUS_READY);
}

/* This function erases an entire sector instead of just a page */
/* because this makes it easier to comply with the 20k page     */
/* write levelling requirement.                                 */
void extmem_erase(uint24 address) {
  address = address & ERASE_MASK;

  spi_set_ss(SPISS_LOW);
  spi_exchange_byte(CMD_SECTOR_ERASE);
  spi_send_24bit(address);
  spi_set_ss(SPISS_HIGH);

  while (extmem_write_isbusy()) ;

  if (address == 0) {
    /* erase sector 0b too */
    spi_set_ss(SPISS_LOW);
    spi_exchange_byte(CMD_SECTOR_ERASE);
    spi_exchange_byte(0);
    spi_exchange_byte(0x80);
    spi_exchange_byte(0);
    spi_set_ss(SPISS_HIGH);

    while (extmem_write_isbusy()) ;
  }
}

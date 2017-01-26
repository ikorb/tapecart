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


   extmem_i2c.c: I2C EEPROM external memory

*/

#include <avr/wdt.h>
#include "config.h"
#include "i2c.h"
#include "extmem.h"

#define PAGE_MASK ((CONFIG_EXTMEM_SIZE - 1) & ~(CONFIG_EXTMEM_PAGE_SIZE - 1))

void extmem_init(void) {
  i2c_init();
}

void extmem_read_start(uint24 address) {
  i2c_start();
  i2c_send_byte(EEPROM_ADDR);
  i2c_send_byte((address >> 8) & 0xff);
  i2c_send_byte( address       & 0xff);
  i2c_stop();
  wdt_reset();

  i2c_start();
  i2c_send_byte(EEPROM_ADDR | 1);
}

uint8_t extmem_read_byte(bool last_byte) {
  return i2c_recv_byte(!last_byte);
}

void extmem_read_stop(void) {
  i2c_stop();
}

void extmem_write_start(uint24 address) {
  i2c_start();
  i2c_send_byte(EEPROM_ADDR);
  i2c_send_byte(address >> 8);
  i2c_send_byte(address & 0xff);
}

void extmem_write_byte(uint8_t byte) {
  i2c_send_byte(byte);
}

void extmem_write_stop(void) {
  i2c_stop();
}

bool extmem_write_isbusy(void) {
  uint8_t tmp;

  i2c_start();
  tmp = i2c_send_byte(EEPROM_ADDR);
  i2c_stop();
  return tmp != 0;
}

void extmem_erase(uint24 address) {
  /* fake a page erase by filling it with 0xff */
  uint16_t i;
  
  extmem_write_start(address & PAGE_MASK);
  
  for (i = 0; i < CONFIG_EXTMEM_ERASE_SIZE; i++) {
    extmem_write_byte(0xff);
  }

  extmem_write_stop();

  while (extmem_write_isbusy()) ;
}

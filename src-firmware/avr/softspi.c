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


   softspi.c: Software SPI master (using USI)

*/

#include <avr/io.h>
#include "config.h"
#include "spi.h"

// Mode 0:
// - initial Clock low
// - master out valid on rising edge
// - master in sample on falling edge

void spi_init(void) {
  SPI_PORT |=  _BV(SPI_SS) | _BV(SPI_DI);
  SPI_PORT &= ~_BV(SPI_SCK);
  SPI_DDR  |=  _BV(SPI_SS) | _BV(SPI_SCK) | _BV(SPI_DO);

  USICR = _BV(USIWM0) | _BV(USICS1) | _BV(USICLK);
}

uint8_t spi_exchange_byte(uint8_t txbyte) {
  USIDR = txbyte;
  for (uint8_t i = 0; i < 16; i++) {
    USICR |= _BV(USITC);
  }
  return USIDR;
}

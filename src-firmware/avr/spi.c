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


   spi.c: SPI implementation for ATmega328P

*/

#include <avr/io.h>
#include "config.h"
#include "spi.h"

// Mode 0:
// - initial Clock low
// - master out valid on rising edge
// - master in sample on falling edge

void spi_init(void) {
  SPI_DDR  |=  _BV(SPI_SS) | _BV(SPI_SCK) | _BV(SPI_DO);
  SPI_DDR  &= ~_BV(SPI_DI);
  SPI_PORT |=  _BV(SPI_SS) | _BV(SPI_SCK) | _BV(SPI_DO) | _BV(SPI_DI);

#ifndef HAVE_SD
  // Set SPI Clock to fosc/16 = 1MHz
  SPCR  =  _BV(SPE) | _BV(MSTR) | _BV(SPR0);
  SPSR &= ~_BV(SPI2X);
#endif
}

uint8_t inline __attribute__((always_inline)) spi_exchange_byte(uint8_t txbyte) {
  SPDR = txbyte;
  while ((SPSR & _BV(SPIF)) == 0);
  return SPDR;
}

#ifdef HAVE_SD
void spi_disable(void) {
  SPCR = 0;
}

void spi_clk_slow(void) {
  // Set SPI Clock to fosc/128 = 125kHz
  SPCR  =  _BV(SPE) | _BV(MSTR) | _BV(SPR1) | _BV(SPR0);
  SPSR &= ~_BV(SPI2X);
}

void spi_clk_fast(void) {
  // Set SPI Clock to fosc/2 = 8MHz
  SPCR  = _BV(SPE) | _BV(MSTR);
  SPSR |= _BV(SPI2X);
}
#endif

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


   spi.c: Minimal polling SPI implementation for SAMD09

*/

#include "config.h"
#include <stdbool.h>
#include "samd09c13a.h"
#include "spi.h"

void spi_init(void) {
  /* connect SERCOM0 to PA09/14/15 */
  PORT->Group[0].PMUX[ 9 / 2].bit.PMUXO = MUX_PA09D_SERCOM0_PAD3;
  PORT->Group[0].PMUX[14 / 2].bit.PMUXE = MUX_PA14C_SERCOM0_PAD0;
  PORT->Group[0].PMUX[15 / 2].bit.PMUXO = MUX_PA15C_SERCOM0_PAD1;
  PORT->Group[0].PINCFG[ 9].bit.PMUXEN = 1;
  PORT->Group[0].PINCFG[14].bit.PMUXEN = 1;
  PORT->Group[0].PINCFG[15].bit.PMUXEN = 1;

  /* set PA08 (SS) to output, high */
  PORT->Group[0].OUTSET.reg = SPI_SS_PORT;
  PORT->Group[0].DIRSET.reg = SPI_SS_PORT;
  
  /* init SERCOM0 in SPI mode, 8 MHz */
  SERCOM0->SPI.CTRLA.reg =
    SERCOM_SPI_CTRLA_DIPO(3)         |
    SERCOM_SPI_CTRLA_MODE_SPI_MASTER;

  while (SERCOM0->SPI.SYNCBUSY.bit.CTRLB) ;
  SERCOM0->SPI.CTRLB.reg = SERCOM_SPI_CTRLB_RXEN;
  SERCOM0->SPI.BAUD.reg = F_CPU / (2 * SPI_BITCLOCK) - 1;

  while (SERCOM0->SPI.SYNCBUSY.bit.ENABLE) ;
  SERCOM0->SPI.CTRLA.bit.ENABLE = 1;
}

uint8_t spi_exchange_byte(uint8_t txbyte) {
  SERCOM0->SPI.DATA.reg = txbyte;
  while (!SERCOM0->SPI.INTFLAG.bit.TXC) ;
  SERCOM0->SPI.INTFLAG.reg = SERCOM_SPI_INTFLAG_TXC;
  return SERCOM0->SPI.DATA.reg;
}

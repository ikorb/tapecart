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


   arch-bitbanging.h: Chip-specific inline functions for accessing the GPIOs

*/

#ifndef ARCH_BITBANGING_H
#define ARCH_BITBANGING_H

#include <stdbool.h>
#include <stdint.h>

#define MOTOR_HANDLER void EIC_Handler(void)

static inline void clear_motor_int(void) {
  EIC->INTFLAG.reg = (1 << MOTOR_EXTINT);
}

static inline void set_read(bool state) {
  // note: read is externally inverted
  if (state)
    PORT_IOBUS->Group[0].OUTCLR.reg = READ_BIT;
  else
    PORT_IOBUS->Group[0].OUTSET.reg = READ_BIT;
}

static inline void set_write_output(bool state) {
  if (state)
    PORT_IOBUS->Group[0].DIRSET.reg = WRITE_BIT;
  else
    PORT_IOBUS->Group[0].DIRCLR.reg = WRITE_BIT;
}

static inline void set_write(bool state) {
  if (state)
    PORT_IOBUS->Group[0].OUTSET.reg = WRITE_BIT;
  else
    PORT_IOBUS->Group[0].OUTCLR.reg = WRITE_BIT;
}

static inline void set_sense(bool state) {
  if (state) {
    PORT_IOBUS->Group[0].OUTSET.reg = SENSE_BIT;
  } else {
    PORT_IOBUS->Group[0].OUTCLR.reg = SENSE_BIT;
  }
}

static inline void set_led(bool state) {
  if (state)
    PORT_IOBUS->Group[0].OUTSET.reg = LED_BIT;
  else
    PORT_IOBUS->Group[0].OUTCLR.reg = LED_BIT;
}

static inline bool get_write(void) {
  return PORT_IOBUS->Group[0].IN.reg & WRITE_BIT;
}

static inline bool get_sense(void) {
  return PORT_IOBUS->Group[0].IN.reg & SENSE_BIT;
}

static inline bool get_motor(void) {
  return PORT_IOBUS->Group[0].IN.reg & MOTOR_BIT;
}

static inline bool get_led(void) {
  return PORT_IOBUS->Group[0].IN.reg & LED_BIT;
}

static inline uint32_t get_port(void) {
  return PORT_IOBUS->Group[0].IN.reg;
}

#define _BV(x) (x)

#endif

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

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>

#define MOTOR_HANDLER ISR(MOTOR_VECT)

static inline void clear_motor_int(void) {}

static void inline __attribute__((always_inline)) set_read(bool state) {
  /* push/pull because there are multiple C2N/1351 versions that use a 7414 */
  if (state)
    READ_PORT |=  _BV(READ_BIT);
  else
    READ_PORT &= ~_BV(READ_BIT);
}

static void inline __attribute__((always_inline)) set_sense(bool state) {
  /* use DDR to simulate a switch to GND, C64 has a pullup */
  if (state)
    SENSE_DDR &= ~_BV(SENSE_BIT);
  else
    SENSE_DDR |=  _BV(SENSE_BIT);
}

static void inline __attribute__((always_inline)) set_led(bool state) {
  /* toggle DDR to avoid a short if the user sets the PROG jumper */
  if (state) {
    LED_DDR  |=  _BV(LED_BIT);
    LED_PORT &= ~_BV(LED_BIT);
  } else {
    LED_DDR  &= ~_BV(LED_BIT);
    LED_PORT |=  _BV(LED_BIT);
  }
}

static bool inline __attribute__((always_inline)) get_write(void) {
  return WRITE_PIN & _BV(WRITE_BIT);
}

static bool inline __attribute__((always_inline)) get_sense(void) {
  return SENSE_PIN & _BV(SENSE_BIT);
}

/* returns true if the C64 wants to turn on the tape motor */
static bool inline __attribute__((always_inline)) get_motor(void) {
#ifdef MOTOR_INVERTED
  return !(MOTOR_PIN & _BV(MOTOR_BIT));
#else
  return  (MOTOR_PIN & _BV(MOTOR_BIT));
#endif
}

static inline uint8_t get_port(void) {
  return WRITE_PIN;
}

#endif

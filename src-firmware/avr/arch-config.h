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


   arch-config.h: Architecture-specific configuration stuff

*/


#ifndef ARCH_CONFIG_H
#define ARCH_CONFIG_H

#include <avr/io.h>

/* I/O port definitions */
/* note: Write and Sense must be on the same port */

#define SENSE_PORT  PORTA
#define SENSE_DDR   DDRA
#define SENSE_PIN   PINA

#define WRITE_PORT  PORTA
#define WRITE_DDR   DDRA
#define WRITE_PIN   PINA

#define READ_PORT   PORTA
#define READ_DDR    DDRA
#define READ_PIN    PINA

#define MOTOR_PORT  PORTA
#define MOTOR_DDR   DDRA
#define MOTOR_PIN   PINA

#define LED_PORT    PORTB
#define LED_DDR     DDRB
#define LED_BIT     2

#define MOTOR_BIT   0
#define MOTOR_VECT  PCINT0_vect
#define MOTOR_INVERTED

/* I2C */
#if CONFIG_HARDWARE_VARIANT == 1
#  define SENSE_BIT   1
#  define WRITE_BIT   7
#  define READ_BIT    5

/* AT45 */
#elif CONFIG_HARDWARE_VARIANT == 2
#  define SENSE_BIT 7
#  define WRITE_BIT 3
#  define READ_BIT  1

#else
#  error "Unknown hardware variant selected"
#endif


#ifdef HAVE_I2C

#  define SOFTI2C_PORT    PORTA
#  define SOFTI2C_DDR     DDRA
#  define SOFTI2C_PIN     PINA
#  define SOFTI2C_BIT_SDA 2
#  define SOFTI2C_BIT_SCL 3

// Half-clock-period delay, minimum 3
#  define SOFTI2C_DELAY 3

/* external EEPROM */
#  define EEPROM_ADDR      0xa0

#elif defined(HAVE_AT45)

#  define SPI_SS  PA2
#  define SPI_SCK PA4
#  define SPI_DO  PA5
#  define SPI_DI  PA6


#else

#  error "Unknown memory type specified"

#endif

static inline void ioport_init(void) {
  WRITE_PORT &= ~_BV(WRITE_BIT); // we have an external pullup
  WRITE_DDR  &= ~_BV(WRITE_BIT);
  READ_PORT  &= ~_BV(READ_BIT);
  READ_DDR   |=  _BV(READ_BIT);
  MOTOR_PORT |=  _BV(MOTOR_BIT);
  MOTOR_DDR  &= ~_BV(MOTOR_BIT);
  SENSE_PORT &= ~_BV(SENSE_BIT);
  SENSE_DDR  |=  _BV(SENSE_BIT);
  LED_PORT   |=  _BV(LED_BIT);
  LED_DDR    &= ~_BV(LED_BIT);

  /* motor PCINT */
  PCMSK0  = _BV(MOTOR_BIT);
  GIMSK  |= _BV(PCIE0);
  GIFR   |= _BV(PCIF0);
}

static inline void ioport_uart_init(void) { }

typedef __uint24 uint24;

#endif

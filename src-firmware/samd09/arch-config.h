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


   arch-config.h: Architecture-specific configuration stuff

*/

#ifndef ARCH_CONFIG_H
#define ARCH_CONFIG_H

#include <stdint.h>
#include "samd09c13a.h"

#define ALLOW_RAMEXEC
#define HAVE_UART

#define READ_BIT       PORT_PA24
#define READ_PIN_NUM   24
#define SENSE_BIT      PORT_PA04
#define SENSE_PIN_NUM  4
#define WRITE_BIT      PORT_PA05
#define WRITE_PIN_NUM  5
#define MOTOR_BIT      PORT_PA02
#define MOTOR_PIN_NUM  2
#define MOTOR_EXTINT   2
#define LED_BIT        PORT_PA25
#define LED_PIN_NUM    25
#define UARTTX_BIT     PORT_PA30
#define UARTTX_PIN_NUM 30
#define UARTRX_BIT     PART_PA31
#define UARTRX_PIN_NUM 31

#define SPI_SS_PORT  PORT_PA08
#define SPI_BITCLOCK 12000000  // maximum allowed rate

#define UART_HANDLER         SERCOM1_Handler
#define UART_SERCOM          SERCOM1
#define UART_SERCOM_IRQN     SERCOM1_IRQn
#define UART_RX_PAD          1
#define UART_TX_PAD          0

static inline void ioport_init(void) {
  /* configure I/O ports */
  PORT->Group[0].CTRL.reg                   = 0xffffffffUL; // enable input synchronizers on all pins
  PORT->Group[0].DIRSET.reg                 = READ_BIT  | SENSE_BIT | LED_BIT;
  PORT->Group[0].OUTCLR.reg                 = READ_BIT  | SENSE_BIT | LED_BIT;
  PORT->Group[0].OUTSET.reg                 = WRITE_BIT;
  PORT->Group[0].PINCFG[SENSE_PIN_NUM].reg  = PORT_PINCFG_INEN | PORT_PINCFG_PULLEN;
  PORT->Group[0].PINCFG[WRITE_PIN_NUM].reg  = PORT_PINCFG_INEN | PORT_PINCFG_PULLEN;
  PORT->Group[0].PINCFG[MOTOR_PIN_NUM].reg  = PORT_PINCFG_INEN;

  /* enable both-edge interrupt for the motor line */
  EIC->CONFIG[0].reg =
    (EIC_CONFIG_SENSE0_BOTH | EIC_CONFIG_FILTEN0) << (4 * MOTOR_EXTINT);
  EIC->INTENSET.reg = (1 << MOTOR_EXTINT);
  EIC->CTRL.reg = EIC_CTRL_ENABLE;

  /* connect motor pin to EIC */
  if (MOTOR_PIN_NUM & 1) {
    PORT->Group[0].PMUX[MOTOR_PIN_NUM / 2].bit.PMUXO = PORT_PMUX_PMUXE_A; // even is unshifted
  } else {
    PORT->Group[0].PMUX[MOTOR_PIN_NUM / 2].bit.PMUXE = PORT_PMUX_PMUXE_A;
  }
  PORT->Group[0].PINCFG[MOTOR_PIN_NUM].bit.PMUXEN = 1;

  /* enable interrupt */
  NVIC_EnableIRQ(EIC_IRQn);
}

/* configure UART I/O ports (disables SWD) */
static inline void ioport_uart_init(void) {
  if (UARTRX_PIN_NUM & 1) {
    PORT->Group[0].PMUX[UARTRX_PIN_NUM / 2].bit.PMUXO = PORT_PMUX_PMUXE_C; // even is unshifted
  } else {
    PORT->Group[0].PMUX[UARTRX_PIN_NUM / 2].bit.PMUXE = PORT_PMUX_PMUXE_C;
  }      
  PORT->Group[0].PINCFG[UARTRX_PIN_NUM].reg = PORT_PINCFG_INEN | PORT_PINCFG_PMUXEN;

  if (UARTTX_PIN_NUM & 1) {
    PORT->Group[0].PMUX[UARTTX_PIN_NUM / 2].bit.PMUXO = PORT_PMUX_PMUXE_C; // even is unshifted
  } else {
    PORT->Group[0].PMUX[UARTTX_PIN_NUM / 2].bit.PMUXE = PORT_PMUX_PMUXE_C;
  }
  PORT->Group[0].PINCFG[UARTTX_PIN_NUM].bit.PMUXEN = 1;
}

typedef uint32_t uint24;

#endif

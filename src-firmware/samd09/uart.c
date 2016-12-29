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


   uart.c: UART access for the ATSAMD09

*/

#include "config.h"
#include "samd09c13a.h"
#include "uart.h"

static char txbuf[1 << CONFIG_UART_TX_BUF_SHIFT];
static volatile unsigned int tx_read_idx,tx_write_idx;

static char rxbuf[1 << CONFIG_UART_RX_BUF_SHIFT];
static volatile unsigned int rx_read_idx, rx_write_idx;

void UART_HANDLER(void) {
  if (UART_SERCOM->USART.INTFLAG.bit.DRE &&
      tx_read_idx != tx_write_idx) {
    /* data register empty, send next char */
    UART_SERCOM->USART.DATA.reg = txbuf[tx_read_idx];
    tx_read_idx = (tx_read_idx+1) & (sizeof(txbuf)-1);
    if (tx_read_idx == tx_write_idx)
      UART_SERCOM->USART.INTENCLR.reg = SERCOM_USART_INTENCLR_DRE;
  }

  if (UART_SERCOM->USART.INTFLAG.bit.RXC) {
    /* receive completed, read char */
    uint32_t rxchar = UART_SERCOM->USART.DATA.reg;
    unsigned int nextidx = (rx_write_idx + 1) & (sizeof(rxbuf) - 1);

    if (nextidx == rx_read_idx)
      /* buffer full, drop character */
      return;

    rxbuf[rx_write_idx] = (char)rxchar;
    rx_write_idx = nextidx;
  }
}

char uart_getc(void) {
  /* wait for data */
  while (!uart_gotc()) ;

  char ch = rxbuf[rx_read_idx++];
  rx_read_idx = rx_read_idx & (sizeof(rxbuf) - 1);
  return ch;
}

bool uart_gotc(void) {
  return rx_read_idx != rx_write_idx;
}

void uart_init(void) {
  /* init UART_SERCOM in UART mode, 115200 bps */
  UART_SERCOM->USART.CTRLA.reg =
    SERCOM_USART_CTRLA_DORD               |
    SERCOM_USART_CTRLA_RXPO(UART_RX_PAD)  |
    SERCOM_USART_CTRLA_TXPO(UART_TX_PAD)  |
    SERCOM_USART_CTRLA_MODE_USART_INT_CLK;

  while (UART_SERCOM->USART.SYNCBUSY.bit.CTRLB) ;
  UART_SERCOM->USART.CTRLB.reg =
    SERCOM_USART_CTRLB_RXEN |
    SERCOM_USART_CTRLB_TXEN;

  UART_SERCOM->USART.BAUD.reg = (int)(65536 * (1 - 16.0 * CONFIG_UART_BAUDRATE / F_CPU));

  while (UART_SERCOM->USART.SYNCBUSY.bit.ENABLE) ;
  UART_SERCOM->USART.CTRLA.bit.ENABLE = 1;

  UART_SERCOM->USART.INTENSET.reg = SERCOM_USART_INTENSET_RXC; // DRE will be set by putc
  
  NVIC_EnableIRQ(UART_SERCOM_IRQN);
}

void uart_putc(char ch) {
  unsigned int tmp = (tx_write_idx+1) & (sizeof(txbuf)-1) ;

  if (tx_read_idx == tx_write_idx && UART_SERCOM->USART.INTFLAG.bit.DRE) {
    /* buffer empty, data register empty -> send immediately */
    UART_SERCOM->USART.DATA.reg = (unsigned char)ch;

  } else {
//#ifdef CONFIG_DEADLOCK_ME_HARDER
    while (tmp == tx_read_idx) ;
//#endif
    /* temporarily disable the UART interrupt while the buffer is updated */
    UART_SERCOM->USART.INTENCLR.reg = SERCOM_USART_INTENCLR_DRE;
    txbuf[tx_write_idx] = ch;
    tx_write_idx = tmp;
    UART_SERCOM->USART.INTENSET.reg = SERCOM_USART_INTENSET_DRE;
  }
}

void uart_putchar(char ch) {
  if (ch == 10)
    uart_putc(13);
  uart_putc(ch);
}

void uart_puthex(uint8_t num) {
  uint8_t tmp;
  tmp = (num & 0xf0) >> 4;
  if (tmp < 10)
    uart_putc('0'+tmp);
  else
    uart_putc('a'+tmp-10);

  tmp = num & 0x0f;
  if (tmp < 10)
    uart_putc('0'+tmp);
  else
    uart_putc('a'+tmp-10);
}

void uart_flush(void) {
  while (tx_read_idx != tx_write_idx) ;
}

void uart_puts(const char *text) {
  while (*text) {
    uart_putc(*text++);
  }
}

void uart_putcrlf(void) {
  uart_putc(13);
  uart_putc(10);
}

bool uart_checkbreak(void) {
  return UART_SERCOM->USART.INTFLAG.bit.RXBRK;
}

void uart_clearbreak(void) {
  UART_SERCOM->USART.INTFLAG.reg = SERCOM_USART_INTFLAG_RXBRK;
}

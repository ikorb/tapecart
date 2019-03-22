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


   uart.c: UART access for the ATmega328p

*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "config.h"
#include "uart.h"

#define BAUD_PRESCALE (((((F_CPU/16) + (CONFIG_UART_BAUDRATE/2)) / (CONFIG_UART_BAUDRATE)) - 1) * 2)

static char txbuf[1 << CONFIG_UART_TX_BUF_SHIFT];
static volatile unsigned int tx_read_idx,tx_write_idx;

static char rxbuf[1 << CONFIG_UART_RX_BUF_SHIFT];
static volatile unsigned int rx_read_idx, rx_write_idx;

ISR(USART_UDRE_vect) {
  if (tx_read_idx != tx_write_idx) {
    /* data register empty, send next char */
    UDR0 = (unsigned char)txbuf[tx_read_idx];

    tx_read_idx = (tx_read_idx+1) & (sizeof(txbuf)-1);
    if (tx_read_idx == tx_write_idx)
      /* disable UDR empty interrupt */
      UCSR0B &= ~_BV(UDRIE0);
  }
}

ISR(USART_RX_vect) {
  /* receive completed, read char */
  unsigned char rxchar = UDR0;
  unsigned int nextidx = (rx_write_idx + 1) & (sizeof(rxbuf) - 1);

  if (nextidx == rx_read_idx)
  {
    /* buffer full, drop character */
    return;
  }

  rxbuf[rx_write_idx] = (char)rxchar;
  rx_write_idx = nextidx;
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
  UBRR0 = BAUD_PRESCALE;
  UCSR0A = _BV(U2X0);
  /* 8 data bits, no parity, and 1 stop bit */
  UCSR0C =_BV(UCSZ01)|_BV(UCSZ00);
  /* enable RX interrupt, and RX and TX */
  UCSR0B = _BV(RXCIE0) | _BV(RXEN0) | _BV(TXEN0);
}

void uart_putc(char ch) {
  unsigned int tmp = (tx_write_idx+1) & (sizeof(txbuf)-1);

  if (tx_read_idx == tx_write_idx && (UCSR0A & _BV(UDRE0))) {
    /* buffer empty, data register empty -> send immediately */
    UDR0 = (unsigned char)ch;

  } else {
//#ifdef CONFIG_DEADLOCK_ME_HARDER
    while (tmp == tx_read_idx) ;
//#endif
    /* temporarily disable the UART interrupt while the buffer is updated */
    UCSR0B &= ~_BV(UDRIE0);
    txbuf[tx_write_idx] = ch;
    tx_write_idx = tmp;
    UCSR0B |= _BV(UDRIE0);
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
  /* not supported */
  return false;
}

void uart_clearbreak(void) {
  /* not supported */
}

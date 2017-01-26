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


   uart.h: Definitions for UART access routines

*/

#ifndef UART_H
#define UART_H

#include <stdbool.h>
#include <stdint.h>
#include "config.h"

#ifdef HAVE_UART
void uart_init(void);
char uart_getc(void);
bool uart_gotc(void);
void uart_putc(char c);
void uart_puthex(uint8_t num);
void uart_flush(void);
void uart_puts(const char *text);
void uart_putcrlf(void);
bool uart_checkbreak(void);
void uart_clearbreak(void);

#else
static inline void uart_init(void)           {}
static inline char uart_getc(void)           { return 0; }
static inline bool uart_gotc(void)           { return false; }
static inline void uart_putc(char c)         {}
static inline void uart_puthex(uint8_t b)    {}
static inline void uart_flush(void)          {}
static inline void uart_puts(const char * s) {}
static inline void uart_putcrlf(void)        {}
static inline bool uart_checkbreak(void)     { return false; }
static inline void uart_clearbreak(void)     {}

#endif

#endif

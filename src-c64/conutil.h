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


   conutil.h: a few convenient routines for a text mode user interface

*/

#ifndef CONUTIL_H
#define CONUTIL_H

#include <stdbool.h>
#include <stdint.h>

#define KEY_DEL    0x14
#define KEY_INS    0x94
#define KEY_UP     0x91
#define KEY_DOWN   0x11
#define KEY_LEFT   0x9d
#define KEY_RIGHT  0x1d
#define KEY_RETURN 0x0d
#define KEY_STOP   0x03

#define STATUS_START 18

extern bool input_aborted;

bool read_string(char *buffer, unsigned char maxlen,
                 unsigned char xpos, unsigned char ypos);
uint32_t read_uint(uint32_t preset, unsigned char width,
                   unsigned char xpos, unsigned char ypos);
unsigned char show_menu(unsigned char count, const char **items, unsigned char sel,
                        unsigned char xpos, unsigned char ypos);
void wait_key(void);

void clear_mainarea(void); // keeps status intact
void clear_mainarea_full(void); // also clears status

#endif

/* tapecart - a tape port storage pod for the C64

   Copyright (C) 2013-2017  Ingo Korb <ingo@akana.de>
   All rights reserved.

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


   cmdmode.h: definitions for the command mode

*/

#ifndef CMDMODE_H
#define CMDMODE_H

#ifndef HAVE_SD
#include "arch-eeprom.h"
#else
#include "sd.h"
#endif

void c64command_handler(void);
void uartcommand_handler(void);

/* MCU EEPROM data struct */
typedef struct {
  uint16_t dummy;           // dummy bytes
  uint8_t  filename[16];    // file name (in PETSCII, space-padded)
  uint16_t dataofs;         // offset of data block for simple loader
  uint16_t datalen;         // length of data block for simple loader
  uint16_t calladdr;        // code start address for C64
  uint8_t  loadercode[171]; // C64 fastloader code
} mcu_eeprom_t;

extern mcu_eeprom_t EEMEM mcu_eeprom;

#endif

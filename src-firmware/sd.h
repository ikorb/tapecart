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


   sd.h: Definitions for SD card access

*/

#ifndef SD_H
#define SD_H

#include <stdbool.h>
#include "diskio.h"

typedef enum {
  FILE_NONE = 0x00,
  FILE_DIR,

  FILE_PRG,
  FILE_TCRT,

  FILE_SID,

  FILE_UNKNOWN = 0xFF
} file_t;

typedef enum {
  FILE_SUB_NONE = 0x00,

  FILE_SUB_P00
} file_sub_t;

typedef struct {
  uint8_t type;
  uint24 size;
  char name[16];
} file_info_t;

uint8_t get_file_type(char *filename, uint8_t *sub_type);
bool    select_file(char *filename);

static inline uint8_t eeprom_read_byte(void *ptr) {
  uint8_t *u8ptr = ptr;
  return *u8ptr;
}

static inline uint16_t eeprom_read_word(void *ptr) {
  uint16_t *u16ptr = ptr;
  return *u16ptr;
}

static inline void eeprom_write_byte(void *ptr, uint8_t val) {
  uint8_t *u8ptr = ptr;
  *u8ptr = val;
}

static inline void eeprom_write_word(void *ptr, uint16_t val) {
  uint16_t *u16ptr = ptr;
  *u16ptr = val;
}

static inline void eeprom_safety(void) {}

void eeprom_flush_loader(void);
void eeprom_flush_loadinfo(void);

#endif

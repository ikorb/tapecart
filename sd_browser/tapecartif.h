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


   tapecartif.h: Definitions for the interface to the tapecart

*/

#ifndef TAPECARTIF_H
#define TAPECARTIF_H

#include <stdint.h>
#include <stdbool.h>
#include "../src-firmware/commands.h"

#define FILENAME_LENGTH 16
#define LOADER_LENGTH   171

bool     tapecart_cmdmode(void);
void     tapecart_streammode(void);
void     tapecart_sendbyte(uint8_t byte);
uint8_t  tapecart_getbyte(void);
uint8_t  tapecart_getbyte_fast(void);

void     tapecart_send_u24(uint32_t value);
void     tapecart_send_u16(uint16_t value);

uint32_t tapecart_get_u24(void);
uint16_t tapecart_get_u16(void);

void     tapecart_read_flash(uint32_t offset, uint16_t len, void *data);
void     tapecart_read_flash_fast(uint32_t offset, uint16_t len, void *data);
void     tapecart_load_and_run(uint32_t offset, uint16_t len, uint16_t calladdr);
void     tapecart_write_flash(uint32_t offset, uint16_t len, const void *data);
void     tapecart_erase_flashblock(uint32_t offset);
void     tapecart_erase_64k(uint32_t offset);

void     tapecart_read_loader(uint8_t *data);
void     tapecart_write_loader(const uint8_t *data);
void     tapecart_read_loadinfo(uint16_t *offset, uint16_t *length, uint16_t *calladdr, char *filename);
void     tapecart_write_loadinfo(const uint16_t offset,
                                 const uint16_t length,
                                 const uint16_t calladdr,
                                 const char    *filename);

void     tapecart_read_sizes(uint32_t *total_size, uint16_t *page_size, uint16_t *erase_pages);

void     tapecart_open_dir(char *path, char *new_path);

#endif

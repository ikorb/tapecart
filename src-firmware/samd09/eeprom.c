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


   eeprom.c: Project-specific EEPROM emulation in main flash

*/

#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include "config.h"
#include "cmdmode.h"
#include "arch-eeprom.h"

#define PAGE_SIZE 64

typedef union {
  struct {
    uint32_t used;
    uint8_t  filename[16];
    uint16_t dataofs;
    uint16_t datalen;
    uint16_t calladdr;
  };
  uint8_t padding[PAGE_SIZE];
} loadinfo_page_t;

_Static_assert(sizeof(loadinfo_page_t) == PAGE_SIZE, "loadinfo_page_t must be exactly page-sized");

static __attribute__((section(".eeprom"),aligned(4))) struct {
  loadinfo_page_t loadinfo[4];
  uint8_t         loadercode[256];
} flash_data;

_Static_assert(sizeof(flash_data) == 8*PAGE_SIZE, "flash_data must have a size of 8 pages");

static union {
  loadinfo_page_t loadinfo;
  uint8_t         loadercode[256];
} __attribute__((aligned(4))) flashbuffer;

static void wait_nvmctrl_ready(void) {
  while (!NVMCTRL->INTFLAG.bit.READY) ;
}

static void erase_row(void *addr) {
  NVMCTRL->ADDR.reg  = (uint32_t)addr / 2;
  NVMCTRL->CTRLA.reg =
    NVMCTRL_CTRLA_CMDEX_KEY |
    NVMCTRL_CTRLA_CMD_ER;

  wait_nvmctrl_ready();
}

void eeprom_flush_loader(void) {
  erase_row(flash_data.loadercode);
  memset(flashbuffer.loadercode, 0xff, sizeof(flashbuffer.loadercode));
  memcpy(flashbuffer.loadercode, mcu_eeprom.loadercode, sizeof(mcu_eeprom.loadercode));
  memcpy(flash_data.loadercode, flashbuffer.loadercode, 256);
  wait_nvmctrl_ready();
}

void eeprom_flush_loadinfo(void) {
  unsigned int page;

  for (page = 0; page < 4; page++) {
    if (flash_data.loadinfo[page].used)
      break;
  }

  if (page > 3) {
    /* no empty slots available, erase the entire row */
    erase_row(flash_data.loadinfo);
    page = 0;
  }

  /* create in-RAM version */
  memset(&flashbuffer.loadinfo, 0xff, sizeof(flashbuffer.loadinfo));
  flashbuffer.loadinfo.used = 0;
  memcpy(&flashbuffer.loadinfo.filename, mcu_eeprom.filename, sizeof(mcu_eeprom.filename));
  flashbuffer.loadinfo.dataofs  = mcu_eeprom.dataofs;
  flashbuffer.loadinfo.datalen  = mcu_eeprom.datalen;
  flashbuffer.loadinfo.calladdr = mcu_eeprom.calladdr;

  /* write the remaining data */
  memcpy(&flash_data.loadinfo[page], &flashbuffer.loadinfo, sizeof(flashbuffer.loadinfo));

  wait_nvmctrl_ready();
}

void eeprom_emu_init(void) {
  memset(&mcu_eeprom, 0xff, sizeof(mcu_eeprom_t));

  for (unsigned int i = 0; i < 4; i++) {
    if (flash_data.loadinfo[i].used != 0xffffffffUL) {
      memcpy(mcu_eeprom.filename, flash_data.loadinfo[i].filename, sizeof(mcu_eeprom.filename));
      mcu_eeprom.dataofs  = flash_data.loadinfo[i].dataofs;
      mcu_eeprom.datalen  = flash_data.loadinfo[i].datalen;
      mcu_eeprom.calladdr = flash_data.loadinfo[i].calladdr;
    }
  }

  memcpy(mcu_eeprom.loadercode, flash_data.loadercode, sizeof(mcu_eeprom.loadercode));
}

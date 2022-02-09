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


   extmem_sd.c: SD card as external memory

*/

#include <string.h>
#include "config.h"
#include "extmem.h"
#include "cmdmode.h"

#define PAGE_MASK ((CONFIG_EXTMEM_SIZE - 1) & ~(CONFIG_EXTMEM_PAGE_SIZE - 1))

#define TCRT_LOADINFO 18
#define TCRT_LOADER_FLAG 40
#define TCRT_FLASH_CONTENT_LENGTH 212
#define TCRT_FLASH_CONTENT 216
#define LOADER_VALID_FLAG 1

#define P00_HEADER_SIZE 26

static const FLASH uint8_t default_loader[171] = {
  #include "../src-c64/loader.h"
};

static const uint8_t basic_starter[] = {
  0xfa, 0x07,       // load at $07fa
  0x20, 0x59, 0xa6, // jsr $a659    ; set basic pointer and CLR
  0x4c, 0xae, 0xa7, // jmp $a7ae    ; RUN
  0x00              // $0800 must be zero
};

static uint8_t starter_index;

static FATFS fat_fs;
static FIL file;
static file_t file_type;
static file_sub_t file_sub_type;
static bool use_starter;

void extmem_init(void) {
  file_type = FILE_NONE;
  file_sub_type = FILE_SUB_NONE;
  use_starter = false;
  memset(&mcu_eeprom, 0xff, sizeof(mcu_eeprom_t));
}

static bool compare_extension(char *ext1, const char *ext2) {
  for (uint8_t i = 0; i < 3; i++) {
    if (ext1[i] >= 'a' && ext1[i] <= 'z') {
      if ((ext1[i] - 0x20) != ext2[i]) {
        return false;
      }
    } else if (ext1[i] != ext2[i]) {
      return false;
    }
  }

  return true;
}

uint8_t get_file_type(char *filename, uint8_t *sub_type) {
  uint8_t length = 0;
  uint8_t extension = 16;

  for (; length < 16; length++) {
    uint8_t chr = filename[length];
    if (chr) {
      if (chr == '.') {
        extension = length;
      }
    } else {
      break;
    }
  }

  if (extension > length) {
    extension = length;
  }

  uint8_t type = FILE_UNKNOWN;
  uint8_t sub = FILE_SUB_NONE;

  uint8_t extension_length = length - extension;
  if (extension_length == 0) {
    type = FILE_PRG;  // treat extensionless files as PRG
  } else if (extension_length >= 4) {
    filename += extension + 1;

    if (compare_extension(filename, "PRG")) {
      type = FILE_PRG;
    } else if (compare_extension(filename, "P00")) {
      type = FILE_PRG;
      sub = FILE_SUB_P00;
    } else if (compare_extension(filename, "TCR")) {
      type = FILE_TCRT;
    } else if (compare_extension(filename, "SID")) {
      type = FILE_SID;
    }
  }

  if (sub_type) {
    *sub_type = sub;
  }

  return type;
}

static FRESULT file_seek(uint24 address) {
  FRESULT fr = f_lseek(&file, address);
  if (fr != FR_OK) {
    f_close(&file);
    file_type = FILE_NONE;
  }

  return fr;
}

static FRESULT read_tcrt_info(void) {
  uint8_t load_info[23];

  FRESULT fr = file_seek(TCRT_LOADINFO);
  if (fr == FR_OK) {
    UINT br;
    fr = f_read(&file, load_info, sizeof(load_info), &br);
    if (fr == FR_OK && br == sizeof(load_info)) {
      memcpy(&mcu_eeprom.dataofs, load_info, sizeof(mcu_eeprom.dataofs));
      memcpy(&mcu_eeprom.datalen, &load_info[2], sizeof(mcu_eeprom.datalen));
      memcpy(&mcu_eeprom.calladdr, &load_info[4], sizeof(mcu_eeprom.calladdr));
      memcpy(mcu_eeprom.filename, &load_info[6], sizeof(mcu_eeprom.filename));

      if (load_info[22] & LOADER_VALID_FLAG) {
        if (f_read(&file, mcu_eeprom.loadercode, sizeof(default_loader), &br) != FR_OK ||
            br != sizeof(default_loader)) {
          fr = FR_INVALID_OBJECT;
          f_close(&file);
        }
      } else {
        FLASH_MEMCPY(mcu_eeprom.loadercode, default_loader, sizeof(default_loader));
      }
    } else {
      fr = FR_INVALID_OBJECT;
      f_close(&file);
    }
  }

  return fr;
}

static void set_loadinfo(char *filename, uint16_t size, uint16_t calladdr) {
  if (*filename == '/') {
    filename++; // skip leading slash
  }

  for (uint8_t i = 0; i < sizeof(mcu_eeprom.filename); i++) {
    char src = *filename;
    if (src != 0) {
      if (src >= 'a' && src <= 'z') {
        src -= 0x20;  // To uppercase
      }
      mcu_eeprom.filename[i] = src;
      filename++;
    } else {
      mcu_eeprom.filename[i] = ' ';
    }
  }

  mcu_eeprom.dataofs = 0;
  mcu_eeprom.datalen = size;
  mcu_eeprom.calladdr = calladdr;

  FLASH_MEMCPY(mcu_eeprom.loadercode, default_loader, sizeof(default_loader));
}

static FRESULT read_prg_info(char *filename) {
  UINT br;
  uint16_t load_addr, calladdr;

  FSIZE_t size = f_size(&file);
  if (file_sub_type == FILE_SUB_P00) {
    if (size > P00_HEADER_SIZE && f_lseek(&file, P00_HEADER_SIZE) == FR_OK) {
      size -= P00_HEADER_SIZE;
    } else {
      size = 0;
    }
  }

  if (size > sizeof(load_addr) &&
      f_read(&file, &load_addr, sizeof(load_addr), &br) == FR_OK &&
      br == sizeof(load_addr)) {

    if (load_addr == 0x0801) {
      // add stub to auto start the PRG
      size += sizeof(basic_starter) - 2;
      calladdr = 0x801 - (sizeof(basic_starter) - 2);
      use_starter = true;
    } else {
      calladdr = load_addr;
      use_starter = false;
    }

    set_loadinfo(filename, size, calladdr);
    return FR_OK;
  } else {
    f_close(&file);
    return FR_INVALID_OBJECT;
  }
}

static FRESULT read_file_info(char *filename) {
  FSIZE_t size = f_size(&file);
  set_loadinfo(filename, size, 0x801);

  return FR_OK;
}

static FRESULT open_file(char *filename) {
  FRESULT fr = FR_NO_FILE;
  uint8_t type = get_file_type(filename, &file_sub_type);

  if (type == FILE_TCRT) {
    fr = f_open(&file, filename, FA_READ|FA_WRITE);
  }
  if (fr != FR_OK) {
    fr = f_open(&file, filename, FA_READ);
  }

  if (fr == FR_OK) {
    switch (type) {
      case FILE_PRG:
        fr = read_prg_info(filename);
        break;

      case FILE_TCRT:
        fr = read_tcrt_info();
        break;

      default:
        fr = read_file_info(filename);
        break;
    }

    if (fr == FR_OK) {
      file_type = type;
    }
  }

  return fr;
}

bool select_file(char *filename) {
  FRESULT fr = FR_OK;

  if (file_type) {
    f_close(&file);
    file_type = FILE_NONE;
  } else {
    fr = f_mount(&fat_fs, "", 1);
  }

  if (fr == FR_OK) {
    fr = open_file(filename);
  }

  return fr == FR_OK;
}

void extmem_read_start(uint24 address) {
  switch (file_type) {
    case FILE_NONE:
      break;

    case FILE_PRG:
    {
      uint16_t file_offset = 0;
      if (file_sub_type == FILE_SUB_P00) {
        file_offset = P00_HEADER_SIZE;
      }

      if (use_starter) {
        if (address >= sizeof(basic_starter)) {
          starter_index = sizeof(basic_starter);
          file_seek((address - sizeof(basic_starter)) + 2 + file_offset);
        } else {
          starter_index = address;
          file_seek(2 + file_offset);
        }
      } else {
        starter_index = sizeof(basic_starter);
        file_seek(address + file_offset);
      }
      break;
    }

    case FILE_TCRT:
      file_seek(address + TCRT_FLASH_CONTENT);
      break;

    default:
      file_seek(address);
      break;
  }
}

static uint8_t read_byte_from_file(void) {
  uint8_t result = 0xff;

  UINT br;
  FRESULT fr = f_read(&file, &result, 1, &br);
  if (fr != FR_OK) {
    f_close(&file);
    file_type = FILE_NONE;
  }

  return result;
}

uint8_t extmem_read_byte(bool last_byte) {
  uint8_t result;

  switch (file_type) {
    case FILE_NONE:
      result = 0xff;
      break;

    case FILE_PRG:
      if (starter_index < sizeof(basic_starter)) {
        result = basic_starter[starter_index++];
      } else {
        result = read_byte_from_file();
      }
      break;

    default:
      result = read_byte_from_file();
      break;
  }

  return result;
}

void extmem_read_stop(void) {
}

void extmem_write_start(uint24 address) {
  if (file_type != FILE_TCRT) {
    return;
  }

  file_seek(address + TCRT_FLASH_CONTENT);
}

void extmem_write_byte(uint8_t byte) {
  if (file_type != FILE_TCRT) {
    return;
  }

  UINT br;
  f_write(&file, &byte, 1, &br);
}

void extmem_write_stop(void) {
  if (file_type != FILE_TCRT) {
    return;
  }

  FSIZE_t size = f_size(&file);
  uint32_t content_length = 0;
  if (size > TCRT_FLASH_CONTENT) {
    content_length = size - TCRT_FLASH_CONTENT;
  }

  FRESULT fr = file_seek(TCRT_FLASH_CONTENT_LENGTH);
  if (fr == FR_OK) {
    UINT br;
    f_write(&file, &content_length, sizeof(content_length), &br);
  }

  f_sync(&file);
}

bool extmem_write_isbusy(void) {
  return false;
}

void extmem_erase(uint24 address) {
  /* fake a page erase by filling it with 0xff */
  uint16_t i;

  if (file_type != FILE_TCRT) {
    return;
  }

  extmem_write_start(address & PAGE_MASK);

  for (i = 0; i < CONFIG_EXTMEM_ERASE_SIZE; i++) {
    extmem_write_byte(0xff);
  }

  extmem_write_stop();
}

void eeprom_flush_loadinfo(void) {
  if (file_type != FILE_TCRT) {
    return;
  }

  FRESULT fr = file_seek(TCRT_LOADINFO);
  if (fr == FR_OK) {
    UINT br;
    f_write(&file, &mcu_eeprom.dataofs, sizeof(mcu_eeprom.dataofs), &br);
    f_write(&file, &mcu_eeprom.datalen, sizeof(mcu_eeprom.datalen), &br);
    f_write(&file, &mcu_eeprom.calladdr, sizeof(mcu_eeprom.calladdr), &br);
    f_write(&file, &mcu_eeprom.filename, sizeof(mcu_eeprom.filename), &br);
    f_sync(&file);
  }
}

void eeprom_flush_loader(void) {
  if (file_type != FILE_TCRT) {
    return;
  }

  FRESULT fr = file_seek(TCRT_LOADER_FLAG);
  if (fr == FR_OK) {
    UINT br;
    uint8_t flag = 0;
    if (mcu_eeprom.loadercode[0] != 0) {
      flag = LOADER_VALID_FLAG;
    }

    f_write(&file, &flag, sizeof(flag), &br);
    f_write(&file, &mcu_eeprom.loadercode, sizeof(mcu_eeprom.loadercode), &br);
    f_sync(&file);
  }
}

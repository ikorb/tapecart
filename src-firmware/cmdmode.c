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


   cmdmode.c: the entire command mode

*/

#include <stdbool.h>
#include "config.h"
#include "bitbanging.h"
#include "commands.h"
#include "crc32.h"
#include "debugflags.h"
#include "extmem.h"
#include "system.h"
#include "timer.h"
#include "uart.h"
#include "utils.h"
#include "cmdmode.h"


/* identification string */
#define STRINGIFY1(x) #x
#define STRINGIFY(x)  STRINGIFY1(x)

static const FLASH char idstring[] = "tapecart v" VERSION " " MEMTYPE_STRING;

#ifdef HAVE_SD
static DIR dir;
static bool dir_open = false;
static bool send_dot_dot = false;
#endif

#ifdef ALLOW_RAMEXEC
static uint8_t update_code[256] __attribute__((section(".ramexec")));
extern void __attribute__((noreturn)) __ramexec_start__(uint32_t portbase);
#endif

/* EEPROM struct */
mcu_eeprom_t EEMEM mcu_eeprom;

#ifdef HAVE_UART
/* send/get_byte wrapping */
typedef bool    (*send_byte_t)(uint8_t b);
typedef int16_t (*get_byte_t)(void);
typedef bool    (*check_abort_t)(void);
typedef void    (*send_byte_fast_t)(uint8_t b);
static send_byte_t      send_byte;
static get_byte_t       get_byte;
static check_abort_t    check_abort;
static send_byte_fast_t send_byte_fast;
#else
  #define send_byte      c64_send_byte
  #define get_byte       c64_get_byte
  #define check_abort    get_motor
  #define send_byte_fast fast_sendbyte_cmdmode
#endif

/* debug flags */
uint16_t debug_flags;

/* ASCII-to-PETSCII conversion (for the idstring above) */
static uint8_t asc2pet(uint8_t ch) {
  if (ch > 64 && ch < 91)
    ch += 128;
  else if (ch > 96 && ch < 123)
    ch -= 32;
  else if (ch > 192 && ch < 219)
    ch -= 128;
  return ch;
}


/* async 1-bit byte rx over sense/write, aborts if motor is on */
/* write is clock from C64, bits are read on 0->1, MSB first   */
/* "ready"ness is signalled by setting sense high              */
/* on exit external write is low or motor is on                */
static int16_t c64_get_byte(void) {
  uint8_t v, i, tmp;

  /* signal ready-ness by releasing sense */
  set_sense(true);

  v = 0;
  for (i = 0; i < 8; i++) {
#if CONFIG_HARDWARE_VARIANT != 5  // WRITE_PORT == SENSE_PORT
    do {
      tmp = get_port();

      if (check_abort())
        return -1;
    } while (!(tmp & _BV(WRITE_BIT)));

    v = (v << 1) | !!(tmp & _BV(SENSE_BIT));
#else
    uint8_t tmp_sense;

    do {
      disable_interrupts();
      tmp = get_write();
      tmp_sense = get_sense();
      enable_interrupts();

      if (check_abort())
        return -1;
    } while (!tmp);

    v = (v << 1) | !!tmp_sense;
#endif

    while (get_write() && !check_abort()) ;
  }

  /* wait for a high-to-low edge on sense */
  while (!get_sense())
    if (check_abort())
      return -1;

  while (get_sense())
    if (check_abort())
      return -1;

  /* set sense low to signal "busy" */
  delay_us(10);
  set_sense(false);

  return v;
}

/* async 1-bit byte tx over sense/write, aborts if motor is on */
/* write is clock from C64, bits are read on 0->1, MSB first   */
/* "ready"ness is signalled by setting sense high              */
static bool c64_send_byte(uint8_t v) {
  uint8_t i;
  bool res = false;

  /* signal ready-ness by releasing sense */
  set_sense(true);

  /* wait until write is high (start signal) */
  while (!get_write() && !check_abort()) ;
  if (check_abort())
    return true;

  for (i = 0; i < 8; i++) {
    if (check_abort()) {
      res = true;
      break;
    }

    /* send on 1->0 transition */
    while (get_write() && !check_abort()) ;

    if (v & 0x80)
      set_sense(true);
    else
      set_sense(false);

    v <<= 1;

    /* wait for 0->1 transition */
    while (!get_write() && !check_abort()) ;
  }

  /* wait until write is low again */
  while (get_write() && !check_abort()) ;

  /* set sense low to signal "busy" */
  set_sense(false);

  return res;
}


#ifdef HAVE_UART
/*
 * UART send/get_byte
 */
static bool uart_send_byte(uint8_t b) {
  uart_putc(b);
  return false;
}

static void uart_send_byte_fast(uint8_t b) {
  uart_putc(b);
}

static int16_t uart_get_byte(void) {
  uint8_t ch = (uint8_t)uart_getc();
  //uart_putc(ch); // echo everything
  return ch;
}
#endif


/*
 * command utility subroutines
 */
static bool get_u8(uint8_t *result) {
  int16_t tmp = get_byte();

  if (tmp < 0)
    return true;

  *result = tmp;
  return false;
}

static bool get_u16(uint16_t *result) {
  int16_t tmp;

  /* low byte */
  tmp = get_byte();
  if (tmp < 0)
    return true;
  *result = tmp & 0xff;

  /* high byte */
  tmp = get_byte();
  if (tmp < 0)
    return true;
  *result |= (uint16_t)(tmp & 0xff) << 8;

  return false;
}

static bool get_u24(uint24 *result) {
  int16_t tmp;

  /* low byte */
  tmp = get_byte();
  if (tmp < 0)
    return true;
  *result = tmp & 0xff;

  /* middle byte */
  tmp = get_byte();
  if (tmp < 0)
    return true;
  *result |= (uint24)(tmp & 0xff) << 8;

  /* high byte */
  tmp = get_byte();
  if (tmp < 0)
    return true;
  *result |= (uint24)(tmp & 0xff) << 16;

  return false;
}

static bool send_u16(uint16_t value) {
  if (send_byte(value & 0xff)) return true;
  return send_byte((value >> 8) & 0xff);
}

static bool send_u24(uint24 value) {
  if (send_byte(value & 0xff))        return true;
  if (send_byte((value >> 8) & 0xff)) return true;
  return send_byte((value >> 16) & 0xff);
}


/*
 * command implementations
 */

/* writing to external memory */
static void write_flash(void) {
  /* parameters:
   *   3   address
   *   2   length
   * len   data
   */
  uint16_t len;
  uint24 addr, end;
  bool page_open = false;

  /* receive parameters */
  if (get_u24(&addr))
    return;

  if (get_u16(&len))
    return;

  end = addr + len;

  /* receive and write data */
  while (addr != end) {
    uint8_t tmp;
    if (get_u8(&tmp))
      return;

    /* write byte into I2C EEPROM */
    if (!page_open) {
      extmem_write_start(addr);
      page_open = true;
    }

    extmem_write_byte(tmp);
    addr++;

    if ((addr % CONFIG_EXTMEM_PAGE_SIZE == 0) ||
        addr == end) {
      extmem_write_stop();
      page_open = false;

      /* poll external memory it is ready again */
      bool is_busy;

      do {
        is_busy = extmem_write_isbusy();

        if (check_abort())
          return;
      } while (is_busy);
    }
  }
}

/* erase a block in external memory */
static void erase_flash_block(void) {
  /* parameters:
   *   3 address
   */
  uint24 addr;

  if (get_u24(&addr))
    return;

  extmem_erase(addr);
  while (extmem_write_isbusy() && !check_abort()) ;
}

/* erase a 64KB block in external memory */
static void erase_flash_64k(void) {
  /* parameters:
   *   3 address
   */
  uint24 addr;

  if (get_u24(&addr))
    return;
  addr = addr & (uint24)~0xffffUL;

#if CONFIG_EXTMEM_ERASE_SIZE != 0
  for (uint8_t i = 0; i < 65536 / CONFIG_EXTMEM_ERASE_SIZE; i++) {
    extmem_erase(addr);
    while (extmem_write_isbusy() && !check_abort()) ;
    addr += CONFIG_EXTMEM_ERASE_SIZE;
  }
#endif
}


/* reading from external memory */
static void read_flash(void) {
  /* parameters:
   *   3   address
   *   2   length
   * reply:
   *   len data
   */
  uint16_t len;
  uint24 addr, end;

  /* receive parameters */
  if (get_u24(&addr))
    return;

  if (get_u16(&len))
    return;

  end = addr + len;

  /* send start address to EEPROM and switch to read mode */
  extmem_read_start(addr);

  /* send data */
  while (addr != end) {
    addr++;
    int16_t tmp = extmem_read_byte(addr == end);

    if (tmp < 0) // no provision for aborting from this side
      tmp = 0xff;

    if (send_byte(tmp & 0xff))
      break;
  }

  extmem_read_stop();
}

/* reading from external memory in fast mode */
static void read_flash_fast(void) {
  /* parameters:
   *   3   address
   *   2   length
   * reply:
   *   len data
   */
  uint16_t len;
  uint24 addr, end;

  /* receive parameters */
  if (get_u24(&addr))
    return;

  if (get_u16(&len))
    return;

  end = addr + len;

  /* send start address to EEPROM and switch to read mode */
  extmem_read_start(addr);

  /* send data */
  while (addr != end) {
    addr++;
    int16_t tmp = extmem_read_byte(addr == end);

    if (tmp < 0) // no provision for aborting from this side
      tmp = 0xff;

    if (check_abort())
      break;

    send_byte_fast(tmp & 0xff);
  }

  extmem_read_stop();
}


/* calculate CRC of flash contents */
static void crc32_flash(void) {
  /* parameters:
   *   3   address
   *   3   length
   * reply:
   *   4   crc32
   */
  uint24 addr, len;
  crc_t crc;
  uint8_t databuf[16];

  /* receive parameters */
  if (get_u24(&addr))
    return;

  if (get_u24(&len))
    return;

  /* calculate CRC32 */
  crc = crc_init();
  extmem_read_start(addr);

  while (len > 0) {
    uint8_t i;
    for (i = 0;
         i < sizeof(databuf) && len > 0;
         i++, len--) {
      int16_t tmp = extmem_read_byte(len == 0);
      if (tmp < 0)
        goto loop_end;

      if (check_abort()) {
        extmem_read_stop();
        return;
      }

      databuf[i] = tmp & 0xff;
    }

    crc = crc_update(crc, databuf, i);
  }
 loop_end:
  extmem_read_stop();
  crc = crc_finalize(crc);

  if (send_u16(crc & 0xffff)) return;
  send_u16((crc >> 16) & 0xffff);
}


/* read loader */
static void read_loader(void) {
  /* no parameters
   * reply:
   *   171 loader binary
   */

  for (uint8_t i = 0; i < sizeof(mcu_eeprom.loadercode); i++) {
    if (send_byte(eeprom_read_byte(&mcu_eeprom.loadercode[i])))
      return;
  }
}


/* read load info */
static void read_loadinfo(void) {
  /* no parameters
   * reply:
   *   2 data address
   *   2 data length
   *   2 call address
   *  16 file name
   */

  uint16_t val = eeprom_read_word(&mcu_eeprom.dataofs);
  if (send_u16(val)) return;

  val = eeprom_read_word(&mcu_eeprom.datalen);
  if (send_u16(val)) return;

  val = eeprom_read_word(&mcu_eeprom.calladdr);
  if (send_u16(val)) return;

  for (uint8_t i = 0; i < 16; i++) {
    if (send_byte(eeprom_read_byte(&mcu_eeprom.filename[i])))
      return;
  }
}


/* write loader */
static void write_loader(void) {
  /* parameters:
   *   171 loader code
   * no reply
   */

  for (uint8_t i = 0; i < sizeof(mcu_eeprom.loadercode); i++) {
    uint8_t b;

    if (get_u8(&b))
      return;

    eeprom_write_byte(&mcu_eeprom.loadercode[i], b & 0xff);
  }
  eeprom_flush_loader();
}


/* write load info */
static void write_loadinfo(void) {
  /* parameters:
   *   2 data address
   *   2 data length
   *   2 call address
   *  16 file name
   * no reply
   */

  uint16_t val;

  /* data address */
  if (get_u16(&val))
    return;

  eeprom_write_word(&mcu_eeprom.dataofs, val);

  /* data length */
  if (get_u16(&val))
    return;

  eeprom_write_word(&mcu_eeprom.datalen, val);

  /* call address */
  if (get_u16(&val))
    return;

  eeprom_write_word(&mcu_eeprom.calladdr, val);

  /* file name */
  for (uint8_t i = 0; i < 16; i++) {
    uint8_t v;

    if (get_u8(&v))
      return;

    eeprom_write_byte(&mcu_eeprom.filename[i], v & 0xff);
  }
  eeprom_flush_loadinfo();
}


/* read device info string */
static void read_deviceinfo(void) {
  /* no parameters, sends a C string */
  uint8_t ch;
  const FLASH char *ptr = idstring;

  do {
    ch = asc2pet(*ptr++);
    if (send_byte(ch))
      return;
  } while (ch != 0);
}


/* read device size data */
static void read_devicesizes(void) {
  /* no parameters
   * reply:
   *   3 extmem total size (in byte)
   *   2 extmem page  size (in byte)
   *   2 extmem erase size (in pages - if 0, direct byte write supported)
   */

  if (send_u24(CONFIG_EXTMEM_SIZE))
    return;

  if (send_u16(CONFIG_EXTMEM_PAGE_SIZE))
    return;

  send_u16(CONFIG_EXTMEM_ERASE_SIZE / CONFIG_EXTMEM_PAGE_SIZE);
}


/* read capabilities */
static void read_capabilities(void) {
  /* no parameters
   * reply:
   *  4 extended capability flags
   */
  if (send_u16(0)) return;
  send_u16(0);
}


/* read debug flags */
static void read_debugflags(void) {
  send_u16(debug_flags);
}

/* write debug flags */
static void write_debugflags(void) {
  get_u16(&debug_flags);
}

static uint24   dir_base     = 0;
static uint16_t dir_entries  = 0;
static uint8_t  dir_name_len = 0;
static uint8_t  dir_data_len = 0;


/* set directory parameters */
static void set_dirparams(void) {
  /* parameters:
   *   3 byte directory start address in flash
   *   2 byte number of dir entries
   *   1 byte name length
   *   1 byte data length
   * no reply
   */
  if (get_u24(&dir_base))
    return;

  if (get_u16(&dir_entries))
    return;

  if (get_u8(&dir_name_len))
    return;

  if (dir_name_len > 16)
    dir_name_len = 16;

  get_u8(&dir_data_len);
}

static uint8_t dir_namebuf[16+1];

static void do_dir_lookup(void) {
  /* parameters:
   *   n byte name
   * reply:
   *   1 byte result (0 if name was found, non-0 otherwise)
   *  if result is 0:
   *   m byte data
   */

  /* read name from C64 */
  for (uint8_t i = 0; i < dir_name_len; i++) {
    if (get_u8(dir_namebuf + i))
      return;
  }

  /* find name in flash */
  bool     skip = true;
  uint16_t entry = 0;

  extmem_read_start(dir_base);
  while (skip && entry < dir_entries) {
    skip = false;

    /* check for match in the current dir entry */
    for (uint8_t i = 0; i < dir_name_len; i++) {
      int16_t b = extmem_read_byte(false);

      if (b < 0) {
        /* memory access error */
        send_byte(1);
        goto loop_end;
      }

      /* check for match */
      if (dir_namebuf[i] != b)
        skip = true;
    }

    /* send "found it" mark if needed */
    if (!skip) {
      if (send_byte(0))
        goto loop_end;
    }

    /* skip or send data */
    for (uint8_t i = 0; i < dir_data_len; i++) {
      int16_t b = extmem_read_byte(false);

      if (b < 0)
        b = 0xff;

      if (!skip)
        if (send_byte(b & 0xff))
          goto loop_end;
    }

    entry++;
  }

  if (skip) {
    /* not found */
    send_byte(1);
  }

 loop_end:
  extmem_read_stop();
}

#ifdef HAVE_SD
static char to_petscii(char c) {
  if (c >= 'a' && c <= 'z') {
    c -= 0x20;
  } else if (c >= 'A' && c <= 'Z') {
    c += 0x20;
  }

  return c;
}

static void sd_open_dir(void) {
  /* parameters:
   *   16 byte dir name (null-terminated if shorter than 16 bytes)
   * reply:
   *   16 byte current dir name (null-terminated if shorter than 16 bytes)
   */

  /* read name from C64 */
  for (uint8_t i = 0; i < 16; i++) {
    if (get_u8(dir_namebuf + i))
      return;
  }
  dir_namebuf[16] = 0;

  if (dir_open) {
    f_closedir(&dir);
    dir_open = false;
  }

  char new_dir[64];
  new_dir[0] = 0;
  uint8_t index = 0;

  if (f_chdir((char *)dir_namebuf) == FR_OK &&
      f_opendir(&dir, "") == FR_OK) {
    dir_open = true;
    send_dot_dot = true;

    f_getcwd(new_dir, sizeof(new_dir));

    uint8_t len = 0;
    for (; len < sizeof(new_dir); len++) {
      if (!new_dir[len])
        break;
    }

    if (len == 0) {
      new_dir[0] = '.';
      new_dir[1] = 0;
    } else if (len > 16) {
      index = len - 16;
      new_dir[index] = '.';
      new_dir[index+1] = '.';
    } else if (len == 1 && new_dir[0] == '/') {
      send_dot_dot = false;
    }
  }

  for (uint8_t i = index; i < index + 16; i++) {
    char c = to_petscii(new_dir[i]);
    if (send_byte(c))
      return;
  }
}

static void sd_read_dir(void) {
  /* parameters:
   *   2 byte max number of files (0: no limit)
   * reply:
   *   1 byte file type (0: end of dir, see file_t)
   *   3 byte file size
   *  16 byte filename (null-terminated if shorter than 16 bytes)
   */

  uint16_t max_files;
  if (get_u16(&max_files))
    return;

  for (uint16_t i = 0; max_files == 0 || i < max_files; i++) {
    uint8_t file_type = FILE_NONE;
    uint24 file_size = 0;
    FILINFO info;
    info.fname[0] = 0;

    if (dir_open) {
      if (send_dot_dot) {
        info.fname[0] = '.';
        info.fname[1] = '.';
        info.fname[2] = 0;
        file_type = FILE_DIR;

        send_dot_dot = false;
      } else {
        if (f_readdir(&dir, &info) != FR_OK || !info.fname[0]) {
          f_closedir(&dir);
          dir_open = false;
        } else {
          if (info.fattrib & AM_DIR) {
            file_type = FILE_DIR;
          } else {
            file_type = get_file_type(info.fname, 0);
          }

          file_size = (uint24)(0xFFFFFF & info.fsize);
        }
      }
    }

    send_byte_fast(file_type);

    uint8_t *size_ptr = (uint8_t *)&file_size;
    send_byte_fast(*size_ptr++);
    send_byte_fast(*size_ptr++);
    send_byte_fast(*size_ptr);

    for (uint8_t l = 0; l < 16; l++) {
      char c = to_petscii(info.fname[l]);
      send_byte_fast(c);
    }

    if (file_type == FILE_NONE)
      break;

    if (check_abort())
      break;
  }
}

static void sd_select_file(void) {
  /* parameters:
   *   16 byte dir name (null-terminated if shorter than 16 bytes)
   * no reply
   */

  /* read name from C64 */
  for (uint8_t i = 0; i < 16; i++) {
    if (get_u8(dir_namebuf + i))
      return;
  }

  dir_namebuf[16] = 0;
  select_file((char *)dir_namebuf);
}
#endif

#ifdef ALLOW_RAMEXEC
/* download code and execute it */
/* intended for firmware updates only */
static void start_ramexec(void) {
  /* 256 byte code
   * no reply
   */
  uint8_t *ptr = update_code;

  for (unsigned int i = 0; i < 256; i++) {
    uint8_t tmp;

    if (get_u8(&tmp))
      return;

    *ptr++ = tmp & 0xff;
  }

  disable_interrupts();
  __ramexec_start__(0x41004400UL); // base of PORT module in SAMD09, might save a few bytes
}
#endif


/*
 * command protocol handler
 */
static void command_handler(void) {
  /* accept commands while motor is off */
  while (!check_abort()) {
    if (debug_flags & DEBUGFLAG_SEND_CMDOK) {
      send_byte('O');
      send_byte('K');
    }

    uint8_t cmd;
    if (get_u8(&cmd))
      break;

    if (debug_flags & DEBUGFLAG_BLINK_COMMAND)
      blink_value(cmd, 8);

    switch (cmd) {
    case CMD_EXIT:
    default:
      /* unknown command, exit command handler */
      return;

    case CMD_READ_DEVICEINFO:
      read_deviceinfo();
      break;

    case CMD_READ_DEVICESIZES:
      read_devicesizes();
      break;

    case CMD_READ_CAPABILITIES:
      read_capabilities();
      break;

    case CMD_READ_FLASH:
      read_flash();
      break;

    case CMD_READ_FLASH_FAST:
      read_flash_fast();
      break;

    case CMD_WRITE_FLASH:
      write_flash();
      break;

    case CMD_WRITE_FLASH_FAST:
      // FIXME: Not yet implemented
      return;

    case CMD_ERASE_FLASH_64K:
      erase_flash_64k();
      break;

    case CMD_ERASE_FLASH_BLOCK:
      erase_flash_block();
      break;

    case CMD_CRC32_FLASH:
      crc32_flash();
      break;

    case CMD_READ_LOADER:
      read_loader();
      break;

    case CMD_READ_LOADINFO:
      read_loadinfo();
      break;

    case CMD_WRITE_LOADER:
      write_loader();
      break;

    case CMD_WRITE_LOADINFO:
      write_loadinfo();
      break;

    case CMD_LED_ON:
      set_led(true);
      break;

    case CMD_LED_OFF:
      set_led(false);
      break;

    case CMD_READ_DEBUGFLAGS:
      read_debugflags();
      break;

    case CMD_WRITE_DEBUGFLAGS:
      write_debugflags();
      break;

    case CMD_DIR_SETPARAMS:
      set_dirparams();
      break;

    case CMD_DIR_LOOKUP:
      do_dir_lookup();
      break;

#ifdef HAVE_SD
    case CMD_SD_OPEN_DIR:
      sd_open_dir();
      break;

    case CMD_SD_READ_DIR_FAST:
      sd_read_dir();
      break;

    case CMD_SD_SELECT_FILE:
      sd_select_file();
      break;
#endif

#ifdef ALLOW_RAMEXEC
    case CMD_RAMEXEC:
      start_ramexec();
      break;
#endif
    }

    /* reset current AVR EEPROM address to reduce chance of corruption */
    eeprom_safety();
  }
}

void c64command_handler(void) {
  /* disable pulse ISR */
  pulsetimer_enable(false);
  set_read(false);

  /* set sense high (input w/ external pullup) */
  set_sense(true);

  /* wait until write is high or motor on */
  while (!get_motor() && !get_write()) ;
  if (get_motor())
    return;

  /* send pulses until write is low or motor on */
  pulsetimer_enable(true);
  while (!get_motor() && get_write()) ;
  if (get_motor())
    return;
  pulsetimer_enable(false);

  set_led(true);
  set_read(false);

  /* call command handler */
#ifdef HAVE_UART
  send_byte      = c64_send_byte;
  get_byte       = c64_get_byte;
  check_abort    = get_motor;
  send_byte_fast = fast_sendbyte_cmdmode;
#endif
  command_handler();

  /* reset sense */
  set_sense(true);

  set_led(false);
}

#ifdef HAVE_UART
void uartcommand_handler(void) {
  pulsetimer_enable(false); // FIXME: Figure out why this helps
  ioport_uart_init();

  debug_flags |= DEBUGFLAG_SEND_CMDOK;
  uart_clearbreak();

  send_byte      = uart_send_byte;
  get_byte       = uart_get_byte;
  check_abort    = uart_checkbreak;
  send_byte_fast = uart_send_byte_fast;
  command_handler();

  uart_puts("BYE");

  uart_clearbreak();
}
#endif

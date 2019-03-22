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


   main.c: Streaming and fastload modes and mode switch detection

*/

#include <stdbool.h>
#include <string.h>
#include "config.h"
#include "bitbanging.h"
#include "cmdmode.h"
#include "debugflags.h"
#include "extmem.h"
#include "system.h"
#include "timer.h"
#include "uart.h"
#include "utils.h"
#include "watchdog.h"

#ifdef HAVE_SD
#include "diskio.h"
#endif

/* half-wave pulse timer values */
#define CBMPULSE_SHORT  (0x30 / 2 - 1)
#define CBMPULSE_MEDIUM (0x42 / 2 - 1)
#define CBMPULSE_LONG   (0x56 / 2 - 1)

#define SREG_MAGIC_VALUE_LOADER  0xca65
#define SREG_MAGIC_VALUE_COMMAND 0xfce2

typedef enum {
  MODE_STREAM,
  MODE_LOADER,
  MODE_C64COMMAND
} tapmode_t;

/* pulse buffer */
static volatile uint8_t tap_buffer[1 << CONFIG_TAPBUFFER_SHIFT];
static volatile uint8_t tapbuf_readidx;
static          uint8_t tapbuf_writeidx;

/* motor/write shift register and mode selection */
static volatile uint16_t  shift_reg;
static volatile tapmode_t mode = MODE_STREAM;
static          tapmode_t old_mode = MODE_STREAM;

/* external flash tool detection */
static const char uart_trigger_string[] = "UARTPROG";
extern char __ramexec_start__;


/*** Interrupt handlers ***/

/* timer 0 overflow ISR for sending pulses to the C64 */
PULSETIMER_HANDLER {
  static bool first_half = false;

  if (first_half) {
    /* reset read line and let the timer overflow again */
    set_read(true);

  } else {
    uint8_t len;

    set_read(false);

    /* next pulse */
    len = tap_buffer[tapbuf_readidx];
    tapbuf_readidx = (tapbuf_readidx + 1) & (sizeof(tap_buffer) - 1);
    pulsetimer_set_pulselen(len);
  }

  first_half = !first_half;
}

/* timer 2 overflow ISR for disk timer */
#ifdef HAVE_SD
SD_TIMER_HANDLER {
  disk_timerproc();
}
#endif

/* pinchange ISR for the motor line */
MOTOR_HANDLER {
  if (get_motor()) {
    /* motor is on */
    pulsetimer_enable(true);

    /* capture write line state */
    shift_reg = (shift_reg << 1) | !!get_write();

    /* check for handshake */
    if (shift_reg == SREG_MAGIC_VALUE_LOADER) {
      mode = MODE_LOADER;
    } else if (shift_reg == SREG_MAGIC_VALUE_COMMAND) {
      mode = MODE_C64COMMAND;
    }

  } else {
    /* motor is off */
    pulsetimer_enable(false);
  }

  clear_motor_int();
}


/* ====================================================================== */
/* = CBM tape format generator                                          = */
/* ====================================================================== */

/* add a single pulse in the buffer, waiting for free space if req'd */
static void put_pulse(uint8_t pulse) {
  /* calculate index of next entry */
  uint8_t idx = (tapbuf_writeidx+1) & (sizeof(tap_buffer) - 1);

  /* wait until this index isn't the next-to-be-read */
  /* or the mode changes                             */
  while (idx == tapbuf_readidx) {
    if (mode != MODE_STREAM)
      break;
  }

  tap_buffer[idx] = pulse;
  tapbuf_writeidx = idx;
}

static void cbm_sync(uint16_t length) {
  for (uint16_t i = 0; i < length; i++)
    put_pulse(CBMPULSE_SHORT);
}

static void cbm_bit(uint8_t value) {
  if (value) {
    put_pulse(CBMPULSE_MEDIUM);
    put_pulse(CBMPULSE_SHORT);
  } else {
    put_pulse(CBMPULSE_SHORT);
    put_pulse(CBMPULSE_MEDIUM);
  }
}

static void cbm_byte(uint8_t value) {
  /* marker */
  put_pulse(CBMPULSE_LONG);
  put_pulse(CBMPULSE_MEDIUM);

  bool parity = true;
  for (uint8_t i = 0; i < 8; i++) {
    uint8_t bit = value & 1;

    cbm_bit(bit);
    if (bit)
      parity = !parity;

    value >>= 1;
  }

  cbm_bit(parity);
}

/* data function to transmit a header block */
static void header_datafunc(void) {
  /* transmit leader */
  uint8_t checksum = 0x03 ^ 0x02 ^ 0x03 ^ 0x04 ^ 0x03;
  cbm_byte(0x03); // load to absolute address
  cbm_byte(0x02); // start address
  cbm_byte(0x03);
  cbm_byte(0x04); // end address + 1
  cbm_byte(0x03);

  /* transmit file name */
  for (uint8_t i = 0; i < 16; i++) {
    uint8_t b = eeprom_read_byte(&mcu_eeprom.filename[i]);
    cbm_byte(b);
    checksum ^= b;
  }

  /* transmit remainder of loader */
  uint8_t *eeptr = mcu_eeprom.loadercode;

  for (uint8_t i = 0; i < 192 - 16 - 5; i++) {
    uint8_t b = eeprom_read_byte(eeptr++);
    cbm_byte(b);
    checksum ^= b;
  }

  /* reset current AVR EEPROM address to reduce chance of corruption */
  eeprom_safety();

  cbm_byte(checksum);
}

/* data function to transmit a start vector */
static void vector_datafunc(void) {
  cbm_byte(0x51);
  cbm_byte(0x03);
  cbm_byte(0x51 ^ 0x03);
}


/* transmit a data block */
static void cbm_datablock(void (*datafunc)(void)) {
  for (uint8_t repeat = 0; repeat < 2; repeat++) {
    /* send countdown sequence */
    for (uint8_t i = 9; i > 0; i--) {
      if (repeat)
        cbm_byte(i);
      else
        cbm_byte(i + 0x80);
    }

    datafunc();

    /* transmit block gap */
    put_pulse(CBMPULSE_LONG);
    cbm_sync(60); // note: should be less than the tap_buffer size
  }
}


/* ====================================================================== */
/* = simple fast loader                                                 = */
/* ====================================================================== */

static bool tx_byte(uint8_t b) {
  wdt_reset();
  if (get_motor())
    return true;

  set_led(false);
  fast_sendbyte_loader(b);
  set_led(true);

  return false;
}

/* current state: sense inactive, C64 is waiting for it to become active */
static void loader_handler(void) {
  set_led(true);
  pulsetimer_enable(false);

  /* enable watchdog to handle lockups */
  wdt_enable(WDTO_250MS);

  /* read data offset and len from AVR EEPROM */
  uint16_t dataofs  = eeprom_read_word(&mcu_eeprom.dataofs);
  uint16_t datalen  = eeprom_read_word(&mcu_eeprom.datalen);
  uint16_t calladdr = eeprom_read_word(&mcu_eeprom.calladdr);

  /* reset current AVR EEPROM address to reduce chance of corruption */
  eeprom_safety();

  /* read load address from external memory */
  wdt_reset();
  extmem_read_start(dataofs);
  wdt_reset();

  uint16_t loadaddr, endaddr;

  loadaddr  = extmem_read_byte(false);
  loadaddr |= (uint16_t)extmem_read_byte(false) << 8;
  datalen -= 2;
  wdt_reset();

  /* transmit header block to C64 */
  endaddr = loadaddr + datalen;
  if (tx_byte(calladdr & 0xff)) goto abort;
  if (tx_byte(calladdr >> 8))   goto abort;
  if (tx_byte(endaddr  & 0xff)) goto abort;
  if (tx_byte(endaddr  >> 8))   goto abort;
  if (tx_byte(loadaddr & 0xff)) goto abort;
  if (tx_byte(loadaddr >> 8))   goto abort;

  /* transmit data */
  while (datalen > 0) {
    datalen--;

    uint8_t b = extmem_read_byte(datalen == 0);

    if (tx_byte(b))
      break;
  }

 abort:
  set_led(false);

  /* cleanup */
  extmem_read_stop();
  wdt_disable();
}


/* ====================================================================== */
/* = init and main loop                                                 = */
/* ====================================================================== */

int main(void) {
  /* misc init calls */
  system_init();
  ioport_init();
  uart_init();
  extmem_init();
#ifdef HAVE_SD
  sd_timer_init();
#endif
  pulsetimer_init();
  enable_interrupts();

#if defined(HAVE_UART) && defined(ALLOW_RAMEXEC)
  /* check if an external flash tool is connected */
  if (!memcmp(&__ramexec_start__, uart_trigger_string, strlen(uart_trigger_string))) {
    /* clear the marker string immediately */
    memset(&__ramexec_start__, 0xaa, 16);

    while (1) {
      uartcommand_handler();
    }
  }
#endif

  while (1) {
    old_mode  = mode;
    mode      = MODE_STREAM;
    shift_reg = 0;

#ifdef HAVE_SD
    if (old_mode == MODE_STREAM &&
        !select_file("browser.prg") && !select_file("default.tcrt")) {
      set_sense(true);
      delay_ms(500);
      continue;
    }
#endif

    set_sense(false);

    if (old_mode == MODE_C64COMMAND) {
      // allow fast transition from command to loader mode
      delay_ms(100);
    } else {
      /* transmit header block */
      cbm_sync(1500); // note: 500 appears to be less reliable
      cbm_datablock(header_datafunc);

      /* transmit the autostart vector */
      cbm_sync(1500); // note: 500 does not work
      cbm_datablock(vector_datafunc);

      /* wait until pulse buffer is empty or motor stopped */
      while (get_motor() && (tapbuf_writeidx != tapbuf_readidx)) ;

      /* fill pulse buffer with syncs */
      memset((void *)tap_buffer, CBMPULSE_SHORT, sizeof(tap_buffer));
    }

    /* turn off sense for ~200ms to allow loading again */
    // note: restart isn't reliable if the delay is just 100ms
    set_sense(true);
    for (uint8_t i = 0; i < 200; i++) {
      delay_ms(1);
      if (mode != MODE_STREAM)
        break;
    }

    if (debug_flags & DEBUGFLAG_BLINK_MAGIC)
      blink_value(shift_reg, 16);
 
    /* check if a magic value handshake was received */
    switch (mode) {
    case MODE_LOADER:
      // delay slightly to ensure the C64 has turned the motor signal off
      delay_ms(100);
      loader_handler();
      delay_ms(200);
      break;

    case MODE_C64COMMAND:
      delay_ms(1); // ensure that the motor signal can be turned off
      c64command_handler();
      break;
      
    default:
      break;
    }
  }
}

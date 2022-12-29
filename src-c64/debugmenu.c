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


   debugmenu.c: A few options usually useful for debugging

*/

#include <conio.h>
#include <stdio.h>
#include "conutil.h"
#include "../src-firmware/debugflags.h"
#include "globals.h"
#include "tapecartif.h"
#include "debugmenu.h"

static void hexdump_flash(void) {
  unsigned int i;

  cputsxy(13, 2, "Hexdump flash");
  /* ask for start address */
  //             0123456789012345
  cputsxy(0, 4, "Flash offset: [       ]");
  flash_offset = read_uint(0, true, 7, 15, 4);
  if (input_aborted)
    return;

  /* dump data to screen */
  clear_mainarea();
  gotoxy(0, 2);
  tapecart_read_flash(flash_offset, 128, databuffer);
  for (i = 0; i < 128; i++) {
    cprintf("%02x ", databuffer[i]);
    if ((i & 7) == 7)
      gotoxy(0, 2 + (i+1) / 8);
  }

  wait_key();
  cclearxy(0, STATUS_START - 2, 40);
  cclearxy(0, STATUS_START - 1, 40);
}

static uint32_t data_length;

static void crc32_flash(void) {
  uint32_t crc;

  cputsxy(9, 2, "Calculate flash CRC32");
  cputsxy(0, 4, "Flash offset: [       ]");
  cputsxy(0, 5, "Length      : [       ]");
  flash_offset = read_uint(0, true, 7, 15, 4);
  if (input_aborted)
    return;
  data_length  = read_uint(total_size - 1, true, 7, 15, 5);
  if (input_aborted)
    return;

  tapecart_sendbyte(CMD_CRC32_FLASH);
  tapecart_send_u24(flash_offset);
  tapecart_send_u24(data_length);

  crc  = tapecart_get_u16();
  crc |= (uint32_t)tapecart_get_u16() << 16;

  gotoxy(0, STATUS_START - 2);
  cprintf("CRC32: 0x%08lx", crc);
}

static void set_debugflags(void) {
  uint16_t debugflags;

  cputsxy(12, 2, "Set debug flags");

  tapecart_sendbyte(CMD_READ_DEBUGFLAGS);
  debugflags = tapecart_get_u16();

  gotoxy(0, 4);
  //       012345678901234
  cprintf("Debug flags: [     ]");
  debugflags = read_uint(debugflags, true, 5, 14, 4);
  if (input_aborted)
    return;

  /* remove the ok flag, it would confuse this program */
  debugflags &= ~DEBUGFLAG_SEND_CMDOK;
  
  tapecart_sendbyte(CMD_WRITE_DEBUGFLAGS);
  tapecart_send_u16(debugflags);
}

/*** menu ***/

static const char *debug_menu[] = {
  "1. Hexdump flash",
  "2. Calculate flash CRC32",
  "3. Set debug flags",
  "4. Return to main menu",
};

void debug_tool_menu(void) {
  while (1) {
    current_function = "Debug menu";
    update_top_status();

    clear_mainarea();
    res = show_menu(sizeof(debug_menu) / sizeof(debug_menu[0]),
                    debug_menu, 0, 2, 3);
    clear_mainarea_full();

    switch (res) {
    case 0: // hexdump flash
      hexdump_flash();
      break;

    case 1: // calculate CRC32
      crc32_flash();
      break;
 
    case 2: // set debug flags
      set_debugflags();
      break;

    case 3:
      return;
    }
  }
}


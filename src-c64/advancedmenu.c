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


   advancedmenu.c: "Low level" cart read/write menu items

*/

#include <6502.h>
#include <c64.h>
#include <conio.h>
#include <stdio.h>
#include <string.h>
#include "conutil.h"
#include "globals.h"
#include "io.h"
#include "tapecartif.h"
#include "advancedmenu.h"

int  byteswritten;
long bytesread;

static void write_datafile(void) {
  cputsxy(13, 2, "Write data file");
  //             0123456789012345
  cputsxy(0, 4, "Flash offset: [       ]");
  flash_offset = read_uint(0, true, 7, 15, 4);
  if (input_aborted)
    return;

  if (erase_pages && flash_offset % ((long)page_size * erase_pages)) {
    gotoxy(0, 7);
    cprintf("INFO: Flash will be erased from $%06lx",
            flash_offset & ~((long)page_size * erase_pages - 1L));
  }

  memset(fname, 0, FILENAME_BUFFER_LENGTH);
  //             0123456789012345678901234567890123456789
  cputsxy(0, 5, "File name   : [                        ]");
  if (!read_string(fname, FILENAME_BUFFER_LENGTH - 1, 15, 5, 39 - 15))
    return;

  res = tc_cbm_open(fname);
  if (res != 0) {
    cputsxy(2, STATUS_START - 2, "Failed to open data file");
    return;
  }

  flash_page = flash_offset / page_size;
  pages_erased = 0;
  write_file(-1);

  cputsxy(2, STATUS_START - 2, "Write successful");
  cputsxy(2, STATUS_START - 1, "Remember to set data start+length!");

  tc_cbm_close();
}


/* helper function for dump_flash, dump_tcrt */
void dump_flash_to_file(void) {
  unsigned char i;

  /* blank screen and wait until it is actually blanked */
  VIC.ctrl1 &= ~(1 << 4);
  SEI();
  while (VIC.rasterline >  3) ;
  while (VIC.rasterline <= 3) ;
  while (VIC.rasterline >  3) ;
  CLI();

  flash_offset = 0;
  i = 0;
  while (flash_offset < total_size) {
    if (kbhit()) {
      if (cgetc() == KEY_STOP) {
        cputsxy(2, STATUS_START - 2, "Aborted");
        goto fail;
      }
    }

    bytesread = total_size - flash_offset;
    if (bytesread > sizeof(databuffer))
      bytesread = sizeof(databuffer);

    bordercolor(++i);
    tapecart_read_flash_fast(flash_offset, bytesread, databuffer);
    bordercolor(++i);

    byteswritten = cbm_write(CBM_LFN, databuffer, bytesread);
    if (byteswritten != bytesread) {
      cputsxy(2, STATUS_START - 2, "Failed to write to file");
      goto fail;
    }

    flash_offset += bytesread;
  }

  cputsxy(2, STATUS_START - 2, "Dump successful");

 fail:
  VIC.ctrl1 |= (1 << 4); // un-blank screen
  bordercolor(COLOR_GRAY1);
  cbm_close(CBM_LFN);
}


static void dump_flash(void) {
  memset(fname, 0, FILENAME_BUFFER_LENGTH);

  cputsxy(6, 2, "Dump flash contents to file");
  cputsxy(0, 10, "Note: Screen will be blanked,");
  cputsxy(6, 11, "hold RUN/STOP to abort.");
  //             0123456789012345678901234567890123456789
  cputsxy(0, 4, "File name: [                           ]");
  if (!read_string(fname, FILENAME_BUFFER_LENGTH - 1, 12, 4, 39 - 12))
    return;

  res = cbm_open(CBM_LFN, CURRENT_DEVICE, 1, fname);
  if (res != 0) {
    cputsxy(2, STATUS_START - 2, "Failed to open file");
    return;
  }

  dump_flash_to_file();
}


static void write_default_loader(void) {
  cputsxy(2, 3, "Writing...");

  tapecart_write_loader(default_loader);

  clear_mainarea();
  cputsxy(2, STATUS_START - 2, "Loader updated");
}


static void write_custom_loader(void) {
  memset(fname, 0, FILENAME_BUFFER_LENGTH);
  cputsxy(10, 2, "Write custom loader");
  //             0123456789012345678901234567890123456789
  cputsxy(0, 4, "File name: [                           ]");
  if (!read_string(fname, FILENAME_BUFFER_LENGTH - 1, 12, 4, 39 - 12))
    return;

  res = tc_cbm_open(fname);
  if (res != 0) {
    cputsxy(2, STATUS_START - 2, "Failed to open loader file");
    return;
  }

  cputsxy(2, 6, "Writing...");

  len = tc_cbm_read(databuffer, LOADER_LENGTH + 2);
  if (len != LOADER_LENGTH + 2) {
    gotoxy(2, STATUS_START - 2);
    cprintf("Error: Read only %d byte from the file", len);
    goto fail;
  }

  tapecart_write_loader(databuffer + 2);

  cputsxy(2, STATUS_START - 2, "Loader updated");

 fail:
  tc_cbm_close();
}


static void dump_loader(void) {
  memset(fname, 0, FILENAME_BUFFER_LENGTH);
  cputsxy(10, 2, "Dump loader to file");
  //             0123456789012345678901234567890123456789
  cputsxy(0, 4, "File name: [                           ]");
  if (!read_string(fname, FILENAME_BUFFER_LENGTH - 1, 12, 4, 39 - 12))
    return;

  res = cbm_open(CBM_LFN, CURRENT_DEVICE, 1, fname);
  if (res != 0) {
    cputsxy(2, STATUS_START - 2, "Failed to open file");
    return;
  }

  tapecart_read_loader(databuffer + 2);
  databuffer[0] = 0x51;
  databuffer[1] = 0x03;

  byteswritten = cbm_write(CBM_LFN, databuffer, LOADER_LENGTH + 2);
  if (byteswritten != LOADER_LENGTH + 2) {
    cputsxy(2, STATUS_START - 2, "Failed to write file");
  } else {
    cputsxy(2, STATUS_START - 2, "Dump successful");
  }
  cbm_close(CBM_LFN);
}


static void change_name(void) {
  unsigned char i;
  uint16_t dataofs, datalen, calladdr;

  tapecart_read_loadinfo(&dataofs, &datalen, &calladdr, fname);

  cputsxy(10, 2, "Change display name");
  //                            1234567890123456
  cputsxy(0, 4, "Display name: [                ]");
  if (!read_string(fname, FILENAME_LENGTH, 15, 4, FILENAME_LENGTH))
    return;

  if (strlen(fname) < FILENAME_LENGTH) {
    i = strlen(fname);
    memset(fname + i, ' ', FILENAME_LENGTH - i);
  }

  tapecart_write_loadinfo(dataofs, datalen, calladdr, fname);
}


static void change_bootloc(void) {
  uint16_t dataofs, datalen, calladdr;

  tapecart_read_loadinfo(&dataofs, &datalen, &calladdr, fname);

  cputsxy(8, 2, "Change bootfile location");
  gotoxy(0, 4);
  //       0123456789012345
  cprintf("Data offset : [$%04x]", dataofs);
  gotoxy(0, 5);
  cprintf("Data length : [$%04x]", datalen);
  gotoxy(0, 6);
  cprintf("Call address: [$%04x]", calladdr);

  dataofs  = read_uint(dataofs, true, 5, 15, 4);
  if (input_aborted)
    return;
  datalen  = read_uint(datalen, true, 5, 15, 5);
  if (input_aborted)
    return;
  calladdr = read_uint(calladdr, true, 5, 15, 6);
  if (input_aborted)
    return;

  tapecart_write_loadinfo(dataofs, datalen, calladdr, fname);
}


/*** menu ***/

static const char *advanced_menu_text[] = {
  "1. Write data file to cart",
  "2. Dump flash contents to file",
  "3. Write default loader",
  "4. Write custom loader",
  "5. Dump loader to file",
  "6. Change display name",
  "7. Change bootfile location",
  "8. Return to main menu",
};

void advanced_menu(void) {
  while (1)  {
    display_status();

    clear_mainarea();
    res = show_menu(sizeof(advanced_menu_text) / sizeof(advanced_menu_text[0]),
                    advanced_menu_text, 0, 2, 3);
    clear_mainarea_full();

    switch (res) {
    case 0: // write data file
      write_datafile();
      break;

    case 1: // dump flash contents to file
      dump_flash();
      break;

    case 2: // write default loader
      write_default_loader();
      break;

    case 3: // write custom loader
      write_custom_loader();
      break;

    case 4: // dump loader to file
      dump_loader();
      break;

    case 5: // change name
      change_name();
      break;

    case 6: // change bootfile location
      change_bootloc();
      break;

    case 7:
      return;
    }
  }
}

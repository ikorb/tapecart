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


   drivemenu.c: Drive-related menu items

*/

#include <6502.h>
#include <c64.h>
#include <conio.h>
#include <stdio.h>
#include <string.h>
#include "conutil.h"
#include "globals.h"
#include "drivemenu.h"

static void change_device(void) {
  unsigned char newdevice = current_device;
  
  cputsxy(9, 2, "Change current device");
  cputsxy(0, 10, "Enter 0 to select REU");

  cputsxy(0, 4, "Device number: [  ]");
  do {
    newdevice = read_uint(newdevice, false, 2, 16, 4);
    if (input_aborted)
      return;
  } while ((newdevice != 0 && newdevice < 4) || newdevice > 30);

  current_device = newdevice;
  display_devicenum();
}

static void send_command(void) {
  if (current_device == 0) {
    cputsxy(2, STATUS_START - 2, "Cannot send commands to a REU!");
    return;
  }

  memset(fname, 0, FILENAME_BUFFER_LENGTH);
  cputsxy(14, 2, "Send command");
  //             0123456789012345678901234567890123456789
  cputsxy(0, 4, "Command: [                             ]");
  if (!read_string(fname, FILENAME_BUFFER_LENGTH - 1, 10, 4, 39 - 10))
    return;

  res = cbm_open(15, current_device, 15, fname);
  if (res != 0) {
    cputsxy(2, STATUS_START - 2, "Failed to send command");
    return;
  }

  len = cbm_read(15, fname, FILENAME_BUFFER_LENGTH - 1);
  fname[len] = 0;
  cputsxy(0, STATUS_START - 2, fname);
  cbm_close(15);
}

static uint8_t *bufptr;

static void show_directory(void) {
  unsigned char line = 0;

  if (current_device == 0) {
    cputsxy(2, STATUS_START - 2, "Cannot show directory of a REU!");
    return;
  }

  res = cbm_open(CBM_LFN, current_device, 0, "$");
  if (res != 0) {
    cputsxy(2, STATUS_START - 2, "Failed to open directory");
    return;
  }

  len = cbm_read(CBM_LFN, databuffer, 2);
  if (len != 2) {
    cputsxy(2, STATUS_START - 2, "Failed to read directory");
    goto cleanup;
  }

  clear_mainarea_full();
  gotoxy(0, 2);
  len = 0;
  line = 2;

  while (1) {
    /* fill buffer */
    len += cbm_read(CBM_LFN, databuffer + len, 256 - len);
    databuffer[len] = 0;

    /* pause when screen is full */
    if (line == STATUS_START) {
      if (cgetc() == KEY_STOP) {
        break;
      }

      line = 2;
      clear_mainarea_full();
      gotoxy(0, 2);
    }

    /* check for end marker */
    if (databuffer[0] == 0 && databuffer[1] == 0) {
      wait_key();
      break;
    }

    /* file size */
    cprintf("%u ", (uint16_t)databuffer[2] | ((uint16_t)databuffer[3] << 8));

    /* print until end of line */
    bufptr = databuffer + 4;
    while (*bufptr != 0) {
      if (*bufptr == RVS_ON) {
        revers(1);
      } else {
        cputc(*bufptr);
      }
      bufptr++;
    }

    line++;
    gotoxy(0, line);
    revers(0);

    /* shift remainder of buffer */
    bufptr++;
    len -= bufptr - databuffer;
    memmove(databuffer, bufptr, len);
  }

 cleanup:
  cbm_closedir(CBM_LFN);
  clear_mainarea_full();
}


/*** menu ***/

static const char *drive_menu_text[] = {
  "1. Change current device",
  "2. Send command",
  "3. Show directory",
  "4. Return to main menu",
};

void drive_menu(void) {
  while (1)  {
    display_status();

    clear_mainarea();
    res = show_menu(sizeof(drive_menu_text) / sizeof(drive_menu_text[0]),
                    drive_menu_text, 0, 2, 3);
    clear_mainarea_full();

    switch (res) {
    case 0: // change current device
      change_device();
      break;

    case 1: // send command
      send_command();
      break;

    case 2: // show directory
      show_directory();
      break;

    case 3:
      return;
    }
  }
}

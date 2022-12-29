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
#include "eload.h"
#include "globals.h"
#include "io.h"
#include "drivemenu.h"

static void change_device(void) {
  unsigned char newdevice = CURRENT_DEVICE;

  current_function = "Change device";
  update_top_status();

  cputsxy(0, 10, "Enter 0 to select REU");

  cputsxy(0, 3, "Device number: [  ]");
  do {
    newdevice = read_uint(newdevice, false, 2, 16, 3);
    if (input_aborted)
      return;
  } while ((newdevice != 0 && newdevice < 4) || newdevice > 30);

  set_current_device(newdevice);
}

static void send_command(void) {
  if (CURRENT_DEVICE == 0) {
    cputsxy(2, STATUS_START - 2, "Cannot send commands to a REU!");
    return;
  }

  memset(fname, 0, FILENAME_BUFFER_LENGTH);
  current_function = "Send command";
  update_top_status();

  //             0123456789012345678901234567890123456789
  cputsxy(0, 3, "Command: [                             ]");
  if (!read_string(fname, FILENAME_BUFFER_LENGTH - 1, 10, 3, 39 - 10))
    return;

  res = cbm_open(15, CURRENT_DEVICE, 15, fname);
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

  if (CURRENT_DEVICE == 0) {
    cputsxy(2, STATUS_START - 2, "Cannot show directory of a REU!");
    return;
  }

  res = cbm_open(CBM_LFN, CURRENT_DEVICE, 0, "$");
  if (res != 0) {
    cputsxy(2, STATUS_START - 2, "Failed to open directory");
    return;
  }

  len = cbm_read(CBM_LFN, databuffer, 2);
  if (len != 2) {
    cputsxy(2, STATUS_START - 2, "Failed to read directory");
    goto cleanup;
  }

  current_function = "Directory";
  update_top_status();

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
  "4. Toggle fastloader",
  "5. Return to main menu",
};

void drive_menu(void) {
  while (1)  {
    current_function = "Drive menu";
    update_top_status();
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

    case 3: // toggle fastloader
      if (CURRENT_DEVICE) {
        if (eload_drive_is_fast()) {
          eload_set_drive_disable_fastload(CURRENT_DEVICE);
        } else {
          eload_set_drive_check_fastload(CURRENT_DEVICE);
        }
      }
      break;

    case 4:
      return;
    }
  }
}

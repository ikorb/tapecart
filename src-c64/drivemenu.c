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
  cputsxy(0, 4, "Device number: [  ]");
  do {
    newdevice = read_uint(newdevice, false, 2, 16, 4);
    if (input_aborted)
      return;
  } while (newdevice < 4 || newdevice > 30);

  current_device = newdevice;
  display_devicenum();
}

static void send_command(void) {
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

/*** menu ***/

static const char *drive_menu_text[] = {
  "1. Change current device",
  "2. Send command",
  "3. Return to main menu",
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

    case 2:
      return;
    }
  }
}

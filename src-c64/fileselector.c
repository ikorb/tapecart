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


   fileselector.c: Fancy directory browser for file selection

*/

#include <conio.h>
#include <string.h>
#include "conutil.h"
#include "globals.h"
#include "io.h"
#include "fileselector.h"

#pragma charmap (95, 95) // underscore in source strings is left arrow of C64

#define FILE_TYPE_DEL 0
#define FILE_TYPE_PRG 2
#define FILE_TYPE_DIR 6

/* Offsets in a D64 directory entry */
#define DIR_OFS_FILE_TYPE       2
#define DIR_OFS_TRACK           3
#define DIR_OFS_FILE_NAME       5
#define DIR_OFS_SIZE_LOW        0x1e
#define DIR_OFS_SIZE_HI         0x1f

#define LINES_PER_SCROLL 16

static uint8_t* dataptr;
static uint8_t entries, entry, first_entry, selected_entry;
static uint8_t line, type;

const char filetypes[] = "del\0seq\0prg\0usr\0rel\0cbm\0dir\0???";

static void show_helpline(void) {
  clear_largearea();
  chlinexy(0, 23, 40);
  gotoxy(0, 24);
  //     1234567890123456789012345678901234567890
  cputs("Home/Up/Down/F3/F5/Return/Stop/_/F1:Help");
}

static void show_help(void) {
  clear_largearea();

  gotoxy(0, 3);
  //     1234567890123456789012345678901234567890
  cputs("F1        Show this help screen\n\r"
        "F3        Page up\n\r"
        "F5        Page down\n\r"
        "F7        Send DOS command\n\r"
        "Home      Jump to start of list\n\r"
        "Return    Select file\n\r"
        "Stop      Cancel selection\n\r"
        "_         Move one directory level up\n\r"
        "R         Reload\n\r"
        "\n\r"
        "8/9/0/..7 Select device 8..17\n\r"
        "\n\r"
        "Press any key...");
  
  wait_key();
  show_helpline();
}

static void print_entry(uint8_t* ptr) {
  uint16_t size = (uint16_t)(ptr[DIR_OFS_SIZE_HI]) << 8 | ptr[DIR_OFS_SIZE_LOW];
  uint8_t type = ptr[DIR_OFS_FILE_TYPE];

  cclearxy(6, line, 21);
  gotoxy(0, line);
  cprintf("%5d ", size);

  cputs(ptr + DIR_OFS_FILE_NAME);
  
  gotoxy(22, line);
  if (type & 0x80) {
    cputc(' ');
  } else {
    cputc('*');
  }
  cputs(filetypes + (4 * (type & 0x0f)));
  if (type & 0x40) {
    cputc('<');
  }
}

static void print_entries(void) {
  line = 2;
  dataptr = databuffer + 32 * first_entry;
  entry = first_entry;

  if (entries == 0) {
    gotoxy(0, 2);
    cputs("  (no files, press R to reload)");
    ++line;
  }

  while (line < 23 && entry < entries) {
    if (dataptr[DIR_OFS_FILE_TYPE] != 0) {
      revers(selected_entry == entry);
      print_entry(dataptr);
      revers(0);
      ++line;
    }
    ++entry;
    dataptr += 32;
  }

  while (line < 23) {
    cclearxy(0, line, 40);
    ++line;
  }
}

void select_file(void) {
  memset(fname, 0, FILENAME_BUFFER_LENGTH);
  
  if (CURRENT_DEVICE == 0) {
    // REU does not use a file name
    fname[0] = 'A';
    return;
  }

 restart:
  clear_largearea();
  if (drive_type == DRIVETYPE_UNKNOWN) {
    // use plain text input because the drive may not implement dir reading correctly
    // (e.g. 1541U SoftIEC)
    //             0123456789012345678901234567890123456789
    cputsxy(0, 4, "File name: [                           ]");
    if (!read_string(fname, FILENAME_BUFFER_LENGTH - 1, 12, 4, 39 - 12)) {
      fname[0] = 0;
    }

    return;
  }    
  
  show_helpline();
  res = cbm_open(CBM_LFN, CURRENT_DEVICE, 2, "$");
  if (res != 0) {
    goto exit;
  }

  memset(databuffer, 0, sizeof(databuffer));
  entry = 0;
  dataptr = databuffer;

  // skip BAM
  len = cbm_read(CBM_LFN, dataptr, 254);
  if (len != 254) {
    cbm_close(CBM_LFN);
    goto exit;
  }

  entries = 0;
  if (drive_supports_subdirs) {
    // add a fake <- entry at the beginning for navigation
    entries = 1;
    dataptr[DIR_OFS_FILE_NAME] = '_';
    dataptr[DIR_OFS_FILE_TYPE] = FILE_TYPE_DIR | 0x80;
    dataptr += 32;
  }

  // read raw directory in entry increments
  while (entries < sizeof(databuffer) / 32) {
    gotoxy(0, 2);
    cprintf("reading dir entry %d", entries);

    if (entry) {
      len = cbm_read(CBM_LFN, dataptr, 32);
    } else {
      len = cbm_read(CBM_LFN, dataptr + 2, 30) + 2;
    }

    if (len != 32) {
      break;
    }

    type = dataptr[DIR_OFS_FILE_TYPE];
    if (type) {
      if ((type & 0x20) && drive_type == DRIVETYPE_SD2IEC) {
        // hidden file on sd2iec, ignore
        goto skipfile;
      }
      
      // non-deleted entry
      for (iglobal = DIR_OFS_FILE_NAME; iglobal < DIR_OFS_FILE_NAME + 16; iglobal++) {
        if (dataptr[iglobal] == 0xa0) {
          break;
        }
      }
      dataptr[iglobal] = 0; // write end-of-string into file name or the byte after it

      dataptr += 32;
      entries++;
    }

  skipfile:
    entry++;
    if (entry == 8) {
      entry = 0;
    }
  }

  cbm_close(CBM_LFN);

  cclearxy(0, 2, 40);
  first_entry = 0;
  selected_entry = 0;

  while (1) {
  redraw:
    line = 2;
    dataptr = databuffer + 32 * first_entry;
    entry = first_entry;

    print_entries();
    dataptr = databuffer + 32 * selected_entry;

    while (1) {
      char ch = cgetc();
      switch (ch) {
      case KEY_STOP:
        goto exit;

      case KEY_DOWN:
        if (selected_entry < entries - 1) {
          // un-highlight the current entry
          line = selected_entry - first_entry + 2;
          print_entry(dataptr);

          ++selected_entry;
          ++line;
          if (line == 23) {
            first_entry += LINES_PER_SCROLL;
            goto redraw;
          }
          dataptr += 32;

          // highlight the new entry
          revers(1);
          print_entry(dataptr);
          revers(0);
        }
        break;

      case KEY_UP:
        if (selected_entry > 0) {
          // un-highlight the current entry
          line = selected_entry - first_entry + 2;
          print_entry(dataptr);
          
          --selected_entry;
          --line;
          if (line == 1) {
            if (first_entry < LINES_PER_SCROLL) {
              first_entry = 0;
            } else {
              first_entry -= LINES_PER_SCROLL;
            }
            goto redraw;
          }
          dataptr -= 32;

          // highlight the new entry
          revers(1);
          print_entry(dataptr);
          revers(0);
        }
        break;

      case KEY_HOME:
        selected_entry = 0;
        first_entry = 0;
        goto redraw;

      case KEY_F1:
        // help
        show_help();
        goto redraw;
      
      case KEY_F3:
        // page up
        if (selected_entry >= 21) {
          selected_entry -= 21;
        } else {
          selected_entry = 0;
        }

        if (first_entry >= 21) {
          first_entry -= 21;
        } else {
          first_entry = 0;
        }
        goto redraw;

      case KEY_F5:
        // page down
        selected_entry += 21;
        if (selected_entry >= entries) {
          selected_entry = entries - 1;
        }

        first_entry += 21;
        if (first_entry >= entries) {
          first_entry = entries - 1;
        }
        goto redraw;

      case KEY_RETURN:
        iglobal = dataptr[DIR_OFS_FILE_TYPE] & 0x0f;
        if (iglobal == FILE_TYPE_PRG) {
          strncpy(fname, dataptr + DIR_OFS_FILE_NAME, sizeof(fname));
          goto exit;
        }

        if (iglobal == FILE_TYPE_DIR && drive_supports_subdirs) {
          // mod file name into chdir command
          memcpy(dataptr + DIR_OFS_FILE_NAME - 3, "cd:", 3);
          cbm_open(15, CURRENT_DEVICE, 15, dataptr + DIR_OFS_FILE_NAME - 3);
          cbm_close(15);
          goto restart;
        }
        break;

      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9': {
        uint8_t olddev = CURRENT_DEVICE;
        if (ch < '8') {
          set_current_device(ch - '0' + 10);
        } else {
          set_current_device(ch - '0');
        }
        if (drive_type == DRIVETYPE_NOT_PRESENT) {
          set_current_device(olddev);
        }
        goto restart;
      }

      case 'r':
        send_initialize();
        goto restart;

      default:
        break;
      }
    }
  }

 exit:
  clear_largearea();
  display_status();
}

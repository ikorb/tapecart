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


   flashtool.c: main and way too much other code

*/

#include <6502.h>
#include <c64.h>
#include <conio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "advancedmenu.h"
#include "conutil.h"
#include "debugmenu.h"
#include "drivemenu.h"
#include "eload.h"
#include "globals.h"
#include "io.h"
#include "tapecartif.h"
#include "tcrt.h"
#include "utils.h"

const uint8_t default_loader[LOADER_LENGTH] = {
  #include "loader.h"
};

static const uint8_t basic_starter[] = {
  0x20, 0x60, 0xa6, // jsr $a660
  0x20, 0x8e, 0xa6, // jsr $a68e
  0x4c, 0xae, 0xa7, // jmp $a7ae
  0x00              // $0800 must be zero
};

uint8_t  databuffer[4096];
uint32_t total_size;
uint16_t page_size;
uint16_t erase_pages;
bool loader_is_blank;
bool loader_is_default;

char fname[FILENAME_BUFFER_LENGTH];
char strbuf[16];
const char* current_function;

char tc_fname[FILENAME_LENGTH + 1];
uint16_t tc_loadinfo_offset, tc_loadinfo_length, tc_loadinfo_calladdr, tc_loader_chksum;

long          flash_offset;
uint16_t      flash_page, pages_erased;
size_t        len;
unsigned char res;
uint8_t       iglobal;

void update_status(void) {
  /* read size info */
  tapecart_read_sizes(&total_size, &page_size, &erase_pages);

  /* read data offset/length and file name */
  tapecart_read_loadinfo(&tc_loadinfo_offset, &tc_loadinfo_length, &tc_loadinfo_calladdr, tc_fname);

  for (iglobal = 0; iglobal < FILENAME_LENGTH; ++iglobal) {
    if (tc_fname[iglobal] < 32 || (tc_fname[iglobal] >= 128 && tc_fname[iglobal] <= 159))
      tc_fname[iglobal] = '?';
  }
  tc_fname[FILENAME_LENGTH] = 0;

  /* read loader and calculate checksum */
  tapecart_read_loader(databuffer);

  tc_loader_chksum = 0;
  loader_is_blank = true;
  loader_is_default = true;

  for (iglobal = 0; iglobal < LOADER_LENGTH; ++iglobal) {
    tc_loader_chksum += databuffer[iglobal];
    if (databuffer[iglobal] != 0xff)
      loader_is_blank = false;
    if (databuffer[iglobal] != default_loader[iglobal])
      loader_is_default = false;
  }
}

void display_status(void) {
  chlinexy(0, STATUS_START, 40);
  cputsxy(0, STATUS_START+1, "Current tapecart status:\r\n");

  if (tc_loadinfo_offset != 0xffff) {
    cprintf("  Data start       $%04x\r\n", tc_loadinfo_offset);
    cprintf("  Data length      $%04x\r\n", tc_loadinfo_length);
    cprintf("  Call address     $%04x\r\n", tc_loadinfo_calladdr);
  } else {
    cprintf("  Data not programmed\r\n");
    cclear(40);
    cclear(40);
  }

  cprintf("  Display name    [%-16s]\r\n", tc_fname);

  if (loader_is_blank) {
    cprintf("  Loader not programmed  \r\n");
  } else {
    cprintf("  Loader checksum  ");
    revers(!loader_is_default);
    cprintf("$%04x", tc_loader_chksum);
    revers(0);

    if (loader_is_default) {
      textcolor(COLOR_LIGHTGREEN);
      cprintf(" (default) ");
    } else {
      textcolor(COLOR_LIGHTRED);
      cprintf(" (MODIFIED)");
    }
    textcolor(COLOR_WHITE);
    cprintf("\r\n");
  }
}

void update_top_status(void) {
  chlinexy(0, 1, 40);
  if (current_function) {
    gotoxy(1, 1);
    cputc('\0263');
    cputs(current_function);
    cputc('\0253');
  }

  gotoxy(22, 1);
  if (CURRENT_DEVICE) {
    cprintf("\0263Device: %02d/%s\0253", CURRENT_DEVICE, eload_drive_is_fast() ? "fast" : "slow");
  } else {
    cprintf("\0300\0300\0300\0300\0263Device: REU\0253");
  }
}


/*** main menu items ***/

/* write at most limit bytes to flash, or everything if limit is -1 */
bool write_file(long limit) {
  uint16_t to_read;

  do {
    to_read = page_size;
    if (limit >= 0 && to_read > limit)
      to_read = limit;

    len = tc_cbm_read(databuffer, to_read);

    if (len > 0) {
      if (erase_pages && pages_erased == 0) {
        gotoxy(0, 9);
        cprintf("Erasing page %d [$%06lx]", flash_page, flash_offset);

        tapecart_erase_flashblock(flash_offset);
        pages_erased = erase_pages;
      }

      gotoxy(0, 9);
      cprintf("Writing page %d [$%06lx]", flash_page, flash_offset);
      tapecart_write_flash(flash_offset, len, databuffer);
      flash_offset += len;
      limit        -= len;
    }

    flash_page++;
    pages_erased--;
  } while (len > 0 && limit != 0);

  if (limit > 0)
    return false;
  else
    return true;
}

static uint16_t loadaddr;
static uint16_t calladdr;
static char ch;

static void write_onefiler(void) {
  size_t i;

  memset(fname, 0, FILENAME_BUFFER_LENGTH);
  current_function = "Write onefiler";
  update_top_status();
  //             0123456789012345678901234567890123456789
  cputsxy(0, 4, "File name: [                           ]");
  if (!read_string(fname, FILENAME_BUFFER_LENGTH - 1, 12, 4, 39 - 12))
    return;

  res = tc_cbm_open(fname);
  if (res != 0) {
    cputsxy(2, STATUS_START - 2, "Failed to open data file");
    return;
  }

  len = tc_cbm_read(&loadaddr, 2);
  if (len != 2) {
    cputsxy(2, STATUS_START - 2, "Failed to read load address");
    goto fail;
  }

  gotoxy(0, 6);
  cprintf("Load address is $%04x", loadaddr);

  /* ask about basic starter */
  if (loadaddr == 0x0801) {
    gotoxy(0, 7);
    cprintf("Use BASIC starter? (Y/N) ");

    do {
      ch = cgetc();
    } while (ch != 'y' && ch != 'n');
    cputc(ch);

    if (ch == 'y') {
      /* add a small starter just before $0800 */
      memcpy(databuffer + 2, basic_starter, sizeof(basic_starter));
      loadaddr    -= sizeof(basic_starter);
      calladdr     = loadaddr;
      flash_offset = 2 + sizeof(basic_starter);
      goto skip_calladdr;
    }
  }

  /* ask for call address */
  gotoxy(0, 8);
  //       01234567890123456789
  cprintf("Start program at: [     ]");
  calladdr = read_uint(loadaddr, true, 5, 19, 8);
  if (input_aborted)
    goto fail;
  flash_offset = 2;

 skip_calladdr:
  databuffer[0] = loadaddr & 0xff;
  databuffer[1] = loadaddr >> 8;

  start_timing();

  /* erase first block */
  pages_erased = 0;
  if (erase_pages) {
    gotoxy(0, 9);
    cprintf("Erasing page %d", 0);
    pages_erased = erase_pages - 1; // first page written below
    tapecart_erase_flashblock(0);
  }

  /* read remainder of first block into buffer */
  len = tc_cbm_read(databuffer + flash_offset, page_size - flash_offset);

  /* write it */
  gotoxy(0, 9);
  cprintf("Writing page %d [$%06x]", flash_page, 0);
  tapecart_write_flash(0, flash_offset + len, databuffer);
  flash_offset += len;

  /* write remainder of file */
  flash_page = 1;
  write_file(-1);

  /* write data offset and file name */
  if (strlen(fname) < FILENAME_LENGTH) {
    i = strlen(fname);
    memset(fname + i, ' ', FILENAME_LENGTH - i);
  }

  tapecart_write_loadinfo(0, flash_offset, calladdr, fname);

  cputsxy(2, STATUS_START - 2, "Write completed in ");
  print_timing();

 fail:
  tc_cbm_close();
}


static void display_cartinfo(void) {
  unsigned char i, tmp;

  current_function = "Cart information";
  update_top_status();
  gotoxy(0, 3);
  tapecart_sendbyte(CMD_READ_DEVICEINFO);
  i = 0;
  do {
    tmp = tapecart_getbyte();
    databuffer[i] = tmp;
    i++;
  } while (tmp != 0);

  cprintf("Ident: %s\r\n", databuffer);

  gotoxy(0, 5);
  cprintf("Total size: %7ld byte", total_size);

  gotoxy(0, 6);
  cprintf("Page size : %7d byte", page_size);

  gotoxy(0, 7);
  if (erase_pages) {
    cprintf("Erase size: %7ld byte (%d page%s)",
            (uint32_t)erase_pages * page_size,
            erase_pages,
            (erase_pages == 1) ? "": "s");
  } else {
    cprintf("Byte write access supported");
  }

  cputsxy(0, STATUS_START - 2, "Press any key...");
  wait_key();
  cclearxy(0, STATUS_START - 2, 40);
}


/*** main ***/

static const char *main_menu[] = {
  "1. Write TCRT file to cart",
  "2. Write onefiler to cart",
  "3. Dump cart to TCRT file",
  "4. Advanced options...",
  "5. Display cart info",
  "6. Debug tools...",
  "7. Drive menu...",
  "8. Exit",
};

int main(void) {
  bordercolor(COLOR_GRAY1);
  bgcolor(COLOR_GRAY1);
  textcolor(COLOR_WHITE);

  init_timing();

  if (CURRENT_DEVICE < 4 || CURRENT_DEVICE > 30)
    CURRENT_DEVICE = 8;

  check_fastloader_capability();

  clrscr();
  cputs("tapecart flashtool v" VERSION " by Ingo Korb");
  chlinexy(0, 1, 40);
  chlinexy(0, STATUS_START, 40);
  clear_mainarea_full();

  if (!tapecart_cmdmode()) {
    gotoxy(0, 3);
    cprintf("Error: Tapecart not detected.\n");
    return 0;
  }

  while (1) {
    update_status();
    display_status();
    current_function = NULL;
    update_top_status();

    clear_mainarea();
    res = show_menu(sizeof(main_menu) / sizeof(main_menu[0]),
                    main_menu, 0, 2, 3);
    clear_mainarea_full();

    switch (res) {
    case 0: // write TCRT
      write_tcrt();
      break;

    case 1: // write onefiler
      write_onefiler();
      break;

    case 2: // dump to TCRT
      dump_tcrt();
      break;

    case 3: // Advanced options
      advanced_menu();
      break;

    case 4: // display cart info
      display_cartinfo();
      break;

    case 5: // debug tools submenu
      debug_tool_menu();
      break;

    case 6: // drive submenu
      drive_menu();
      break;

    case 7: // exit
      clrscr();
      return 0;

    default:
      clrscr();
      break;
    }
  }
}

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
#include "globals.h"
#include "tapecartif.h"
#include "tcrt.h"

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

char fname[FILENAME_LENGTH + 1];
char strbuf[16];
unsigned char current_device;

long          flash_offset;
uint16_t      flash_page, pages_erased;
size_t        len;
unsigned char res;


void display_status(void) {
  uint16_t offset, length, calladdr;
  uint16_t wordval;
  unsigned char i, tmp, is_default;

  cputsxy(0, STATUS_START+1, "Current tapecart status:\r\n");

  /* read size info */
  tapecart_read_sizes(&total_size, &page_size, &erase_pages);

  /* read data offset/length and file name */
  tapecart_read_loadinfo(&offset, &length, &calladdr, fname);

  if (offset != 0xffff) {
    cprintf("  Data start       $%04x\r\n", offset);
    cprintf("  Data length      $%04x\r\n", length);
    cprintf("  Call address     $%04x\r\n", calladdr);
  } else {
    cprintf("  Data not programmed\r\n");
    cclear(40);
    cclear(40);
  }

  for (i = 0; i < FILENAME_LENGTH; ++i) {
    if (fname[i] < 32 || (fname[i] >= 128 && fname[i] <= 159))
      fname[i] = '?';
  }
  fname[FILENAME_LENGTH] = 0;

  cprintf("  Display name    [%-16s]\r\n", fname);

  /* read loader and calculate checksum */
  tapecart_read_loader(databuffer);

  wordval = 0;
  tmp = 0; // validity flag
  is_default = 1;

  for (i = 0; i < LOADER_LENGTH; ++i) {
    wordval += databuffer[i];
    if (databuffer[i] != 0xff)
      tmp = 1;
    if (databuffer[i] != default_loader[i])
      is_default = 0;
  }

  if (tmp) {
    cprintf("  Loader checksum  ");
    revers(!is_default);
    cprintf("$%04x", wordval);
    revers(0);

    if (is_default) {
      textcolor(COLOR_LIGHTGREEN);
      cprintf(" (default) ");
    } else {
      textcolor(COLOR_LIGHTRED);
      cprintf(" (MODIFIED)");
    }
    textcolor(COLOR_WHITE);
    cprintf("\r\n");
  } else
    cprintf("  Loader not programmed  \r\n");

}


static void display_devicenum(void) {
  gotoxy(27, STATUS_START);
  cprintf("\0263Device: %02d\0253", current_device);
}


/*** main menu items ***/

/* write at most limit bytes to flash, or everything if limit is -1 */
bool write_file(long limit) {
  uint16_t to_read;

  do {
    to_read = page_size;
    if (limit >= 0 && to_read > limit)
      to_read = limit;

    len = cbm_read(CBM_LFN, databuffer, to_read);

    if (len > 0) {
      if (erase_pages && pages_erased == 0) {
        gotoxy(0, 8);
        cprintf("Erasing page %d [$%06lx]", flash_page, flash_offset);

        tapecart_erase_flashblock(flash_offset);
        pages_erased = erase_pages;
      }

      gotoxy(0, 8);
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

  memset(fname, 0, FILENAME_LENGTH + 1);
  cputsxy(13, 2, "Write onefiler");
  //                             0123456789012345
  cputsxy(0, 4, "File name    : [                ]");
  if (!read_string(fname, FILENAME_LENGTH, 16, 4))
    return;

  res = cbm_open(CBM_LFN, current_device, 0, fname);
  if (res != 0) {
    cputsxy(2, STATUS_START - 2, "Failed to open data file");
    return;
  }

  len = cbm_read(CBM_LFN, &loadaddr, 2);
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
  gotoxy(0, 7);
  //       01234567890123456789
  cprintf("Start program at: [     ]");
  calladdr = read_uint(loadaddr, 5, 19, 7);
  if (input_aborted)
    goto fail;
  flash_offset = 2;

 skip_calladdr:
  databuffer[0] = loadaddr & 0xff;
  databuffer[1] = loadaddr >> 8;

  /* erase first block */
  pages_erased = 0;
  if (erase_pages) {
    gotoxy(0, 8);
    cprintf("Erasing page %d", 0);
    pages_erased = erase_pages - 1; // first page written below
    tapecart_erase_flashblock(0);
  }

  /* read remainder of first block into buffer */
  len = cbm_read(CBM_LFN, databuffer + flash_offset, page_size - flash_offset);

  /* write it */
  gotoxy(0, 8);
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

  cputsxy(2, STATUS_START - 2, "Write successful");

 fail:
  cbm_close(CBM_LFN);
}


static void display_cartinfo(void) {
  unsigned char i, tmp;

  cputsxy(12, 2, "Cart information");
  gotoxy(0, 4);
  tapecart_sendbyte(CMD_READ_DEVICEINFO);
  i = 0;
  do {
    tmp = tapecart_getbyte();
    databuffer[i] = tmp;
    i++;
  } while (tmp != 0);

  cprintf("Ident: %s\r\n", databuffer);

  gotoxy(0, 6);
  cprintf("Total size: %7ld byte", total_size);

  gotoxy(0, 7);
  cprintf("Page size : %7d byte", page_size);

  gotoxy(0, 8);
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
  "1. Write onefiler to cart",
  "2. Write TCRT file to cart",
  "3. Dump cart to TCRT file",
  "4. Advanced options...",
  "5. Display cart info",
  "6. Debug tools...",
  "7. Exit",
};

int main(void) {
  bordercolor(COLOR_GRAY1);
  bgcolor(COLOR_GRAY1);
  textcolor(COLOR_WHITE);

  current_device = *((unsigned char *)0xba);
  if (current_device < 4 || current_device > 30)
    current_device = 8;

  clrscr();
  cputs("tapecart-tool v" VERSION " by Ingo Korb");
  chlinexy(0, 1, 40);
  chlinexy(0, STATUS_START, 40);
  clear_mainarea_full();

  display_devicenum();

  tapecart_cmdmode();

  while (1) {
    display_status();

    clear_mainarea();
    res = show_menu(sizeof(main_menu) / sizeof(main_menu[0]),
                    main_menu, 0, 2, 3);
    clear_mainarea_full();

    switch (res) {
    case 0: // write onefiler
      write_onefiler();
      break;

    case 1: // write TCRT
      write_tcrt();
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

    case 6: // exit
      clrscr();
      return 0;

    default:
      clrscr();
      break;
    }
  }
}

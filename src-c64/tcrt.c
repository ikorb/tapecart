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


   tcrt.c: TCRT read/write

*/

#include <conio.h>
#include <stdio.h>
#include <string.h>
#include "conutil.h"
#include "globals.h"
#include "io.h"
#include "tapecartif.h"
#include "utils.h"
#include "tcrt.h"

const uint8_t tcrt_header[] = {
  0x74, 0x61, 0x70, 0x65,
  0x63, 0x61, 0x72, 0x74,
  0x49, 0x6d, 0x61, 0x67,
  0x65, 0x0d, 0x0a, 0x1a
};

static struct {
  uint16_t dataofs;
  uint16_t datalen;
  uint16_t calladdr;
} loadinfo;

static uint32_t tcrt_flashsize;

void write_tcrt(void) {
  memset(fname, 0, FILENAME_BUFFER_LENGTH);

  cputsxy(12, 2, "Write TCRT to cart");

  //             0123456789012345678901234567890123456789
  cputsxy(0, 4, "File name: [                           ]");
  if (!read_string(fname, FILENAME_BUFFER_LENGTH - 1, 12, 4, 39 - 12))
    return;

  start_timing();

  res = tc_cbm_open(CBM_LFN, current_device, 0, fname);
  if (res != 0) {
    cputsxy(2, STATUS_START - 2, "Failed to open file");
    return;
  }

  bytesread = tc_cbm_read(CBM_LFN, databuffer, TCRT_OFFSET_FLASHDATA);
  if (bytesread != TCRT_OFFSET_FLASHDATA) {
    gotoxy(2, STATUS_START - 2);
    cprintf("Error: Read only %d byte from the file", len);
    goto fail;
  }

  /* sanity-check TCRT data fields */
  if (memcmp(databuffer, tcrt_header, TCRT_HEADER_SIZE)) {
    cputsxy(2, STATUS_START - 2, "Error: File header not recognized");
    goto fail;
  }

  if (databuffer[TCRT_OFFSET_VERSION] != TCRT_VERSION ||
      databuffer[TCRT_OFFSET_VERSION + 1] != 0) {
    cputsxy(2, STATUS_START - 2, "Error: File uses unknown TCRT version");
    goto fail;
  }

  memcpy(&tcrt_flashsize, databuffer + TCRT_OFFSET_FLASHLENGTH, 4);
  if (tcrt_flashsize > total_size) {
    cputsxy(2, STATUS_START - 2, "Error: TCRT flash size exceeds cartridge size");
    goto fail;
  }

  /* copy loader into place if not present */
  if (!(databuffer[TCRT_OFFSET_FLAGS] & TCRT_FLAG_LOADERPRESENT))
    memcpy(databuffer + TCRT_OFFSET_LOADER, default_loader, LOADER_LENGTH);

  /* avoid writing loader if it's identical to the one already in cart */
  tapecart_read_loader(databuffer + TCRT_OFFSET_FLASHDATA);
  if (memcmp(databuffer + TCRT_OFFSET_LOADER,
             databuffer + TCRT_OFFSET_FLASHDATA, LOADER_LENGTH)) {
    tapecart_write_loader(databuffer + TCRT_OFFSET_LOADER);
  }

  memcpy(&loadinfo, databuffer + TCRT_OFFSET_DATAADDR, sizeof(loadinfo));
  tapecart_write_loadinfo(loadinfo.dataofs,
                          loadinfo.datalen,
                          loadinfo.calladdr,
                          databuffer + TCRT_OFFSET_FILENAME);

  flash_offset = 0;
  flash_page   = 0;
  pages_erased = 0;

  if (!write_file(tcrt_flashsize)) {
    cputsxy(2, STATUS_START - 2, "Error: TCRT seems to be truncated");
    goto fail;
  }

  // FIXME: Check for EOF here to find out if TCRT is too long

  /* erase remaining space in cart */
  if (erase_pages) {
    /* page-erase cart */
    flash_offset += page_size * pages_erased;
    while (flash_offset < total_size) {
      gotoxy(0, 8);
      cprintf("Erasing page %d [$%06lx]", flash_page, flash_offset);

      tapecart_erase_flashblock(flash_offset);

      flash_offset += page_size * erase_pages;
      flash_page   += erase_pages;
    }

  } else {
    /* byte-writeable cart */
    memset(databuffer, 0xff, sizeof(databuffer));
    while (flash_offset < total_size) {
      /* note: bytesread is a convenient, though misnamed 32-bit var here */
      bytesread = total_size - flash_offset;
      if (bytesread > sizeof(databuffer))
        bytesread = sizeof(databuffer);

      gotoxy(0, 8);
      cprintf("Erasing offset $%06lx", flash_offset);

      tapecart_write_flash(flash_offset, bytesread, databuffer);
      flash_offset += bytesread;
    }
  }

  cputsxy(2, STATUS_START - 2, "Write completed in ");
  print_timing();

 fail:
  tc_cbm_close(CBM_LFN);
}


void dump_tcrt(void) {
  memset(fname, 0, FILENAME_BUFFER_LENGTH);

  cputsxy(9, 2, "Dump cart to TCRT file");
  cputsxy(0, 10, "Note: Screen will be blanked,");
  cputsxy(6, 11, "hold RUN/STOP to abort.");

  //             0123456789012345678901234567890123456789
  cputsxy(0, 4, "File name: [                           ]");
  if (!read_string(fname, FILENAME_BUFFER_LENGTH - 1, 12, 4, 39 - 12))
    return;

  res = tc_cbm_open(CBM_LFN, current_device, 1, fname);
  if (res != 0) {
    cputsxy(2, STATUS_START - 2, "Failed to open file");
    return;
  }

  memset(databuffer, 0, sizeof(databuffer));
  memcpy(databuffer, tcrt_header, TCRT_HEADER_SIZE);
  databuffer[TCRT_HEADER_SIZE] = TCRT_VERSION;

  tapecart_read_loadinfo(&loadinfo.dataofs,
                         &loadinfo.datalen,
                         &loadinfo.calladdr,
                         fname);
  memcpy(databuffer + TCRT_OFFSET_DATAADDR, &loadinfo, sizeof(loadinfo));
  memcpy(databuffer + TCRT_OFFSET_FILENAME, fname, FILENAME_LENGTH);

  databuffer[TCRT_OFFSET_FLAGS] = TCRT_FLAG_LOADERPRESENT;
  tapecart_read_loader(databuffer + TCRT_OFFSET_LOADER);

  memcpy(databuffer + TCRT_OFFSET_FLASHLENGTH, &total_size, sizeof(total_size));

  byteswritten = tc_cbm_write(CBM_LFN, databuffer, TCRT_OFFSET_FLASHDATA);
  if (byteswritten != TCRT_OFFSET_FLASHDATA) {
    cputsxy(2, STATUS_START - 2, "Failed to write to file");
    tc_cbm_close(CBM_LFN);
    return;
  }

  dump_flash_to_file();
}

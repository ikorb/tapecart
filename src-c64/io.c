/* tapecart - a tape port storage pod for the C64
   All rights reserved.

   This file is copyrighted by markusC64

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

   io.c: cbm_* wrapper functions with read-only REU support

   Note: First four bytes of the REU are assumed to contain the file size.
*/

#include <conio.h>
#include <stdint.h>
#include <stdio.h>
#include "eload.h"
#include "globals.h"
#include "io.h"

uint8_t drive_type;
bool drive_supports_subdirs;
static unsigned long reuSize;
static unsigned long reufilepos;

/* Copy 'size' bytes from REU memory starting at page 'page' offset 'offset' to 'dst' */
static void transfer_from_reu(unsigned long src, unsigned short int size, void *dst) {
  /* FIXME: Use an I/O port struct for the REU registers */
  *(uint16_t *) (0xdf02) = (uint16_t) dst;
  *(uint32_t *) (0xdf04) = src; // FIXME: This clobbers transfer length low
  *(uint16_t *) (0xdf07) = size;
  *(uint8_t  *) (0xdf09) = 0;
  *(uint8_t  *) (0xdf0a) = 0;
  *(uint8_t  *) (0xdf01) = 253;

  /* re-transfer second part of data if the block crosses a 512K boundary */
  if ((src / 0x80000) != ((src+size-1) / 0x80000)) {
    unsigned long offset = 0x80000 - src % 0x80000;

    *(uint16_t *) (0xdf02) = ((uint16_t) dst) + offset;
    *(uint32_t *) (0xdf04) = src + offset; // FIXME: This clobbers transfer length low
    *(uint16_t *) (0xdf07) = size - offset;
    *(uint8_t  *) (0xdf09) = 0;
    *(uint8_t  *) (0xdf0a) = 0;
    *(uint8_t  *) (0xdf01) = 253;
  }
}

/* cbm_open wrapper with reu support, use device 0 to access REU */
unsigned char __fastcall__ tc_cbm_open(const char *name) {
  if (CURRENT_DEVICE) {
    return eload_open_read(name);
  } else {
    reufilepos = 0;

    transfer_from_reu(0, 4, &reuSize);

    return 0;
  }
}

/* cbm_close wrapper with reu support */
void __fastcall__ tc_cbm_close(void) {
  if (CURRENT_DEVICE) {
    eload_close();
  }
}

/* cbm_read wrapper with reu support */
int __fastcall__ tc_cbm_read(void *buffer, unsigned int size) {
  if (CURRENT_DEVICE) {
    return eload_read(buffer, size);
  }

  if (size > reuSize - reufilepos) {
    size = reuSize - reufilepos;
  }

  if (!size) {
    return 0;
  }

  transfer_from_reu(reufilepos + 4, size, buffer);

  reufilepos += size;

  return size;
}

void __fastcall__ check_fastloader_capability(void) {
  if (CURRENT_DEVICE == 0) {
    drive_type = DRIVETYPE_UNKNOWN;
    return;
  }

  drive_type = eload_set_drive_check_fastload(CURRENT_DEVICE);
  if (drive_type == DRIVETYPE_FD ||
      drive_type == DRIVETYPE_HD ||
      drive_type == DRIVETYPE_SD2IEC ||
      drive_type == DRIVETYPE_VICE) {
    drive_supports_subdirs = true;
  } else {
    drive_supports_subdirs = false;
  }

  // try to avoid occasional "disk id mismatch" errors
  if (drive_type != DRIVETYPE_NOT_PRESENT) {
    send_initialize();
  }
}

void __fastcall__ set_current_device(uint8_t dev) {
  CURRENT_DEVICE = dev;
  check_fastloader_capability();
  update_top_status();
}

void __fastcall__ send_initialize(void) {
  if (CURRENT_DEVICE) {
    cbm_open(15, CURRENT_DEVICE, 15, "i");
    cbm_close(15);
  }
}

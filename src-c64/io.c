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
#include <string.h>
#include "io.h"

static signed char   reuLfn = -1;
static unsigned long reuSize;
static unsigned long reufilepos;

#define MAGIC_REU_FILENAME "r:"


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

/* cbm_open wrapper with reu support, use "r:" as filename to access reu */
unsigned char __fastcall__ tc_cbm_open(unsigned char lfn, unsigned char device,
                                       unsigned char sec_addr, const char *name) {
  if (!strcmp(name, MAGIC_REU_FILENAME)) {
    if (reuLfn != -1) {
      return 1;
    }

    reuLfn     = lfn;
    reufilepos = 0;

    transfer_from_reu(0, 4, &reuSize);

    return 0;
  } else {
    return cbm_open(lfn, device, sec_addr, name);
  }
}

/* cbm_close wrapper with reu support */
void __fastcall__ tc_cbm_close(unsigned char lfn) {
  if (lfn == reuLfn) {
    reuLfn = -1;
  } else {
    cbm_close(lfn);
  }
}

/* cbm_read wrapper with reu support */
int __fastcall__ tc_cbm_read(unsigned char lfn, void *buffer, unsigned int size) {
  if (lfn != reuLfn) {
    return cbm_read(lfn, buffer, size);
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

/* cbm_write with reu support - without reu write support */
int __fastcall__ tc_cbm_write(unsigned char lfn, const void *buffer,
                              unsigned int size) {
  if (lfn != reuLfn) {
    return cbm_write(lfn, buffer, size);
  }

  return 0;
}

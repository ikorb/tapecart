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


   globals.h: Declaration of global variables

   (yucky, but better for code efficiency in cc65)

*/

#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdbool.h>
#include <stdint.h>
#include "tapecartif.h" // for LOADER_LENGTH

#define FILENAME_BUFFER_LENGTH 255
#define CBM_LFN 1

/* from flashtool.c */
extern const uint8_t default_loader[LOADER_LENGTH];

extern uint8_t  databuffer[4096];
extern uint32_t total_size;
extern uint16_t page_size;
extern uint16_t erase_pages;

extern char fname[FILENAME_BUFFER_LENGTH];
extern char strbuf[16];

extern unsigned char current_device;

extern long          flash_offset;
extern uint16_t      flash_page, pages_erased;
extern size_t        len;
extern unsigned char res;

void display_status(void);
bool write_file(long limit);


/* from advancedmenu.c */
extern int  byteswritten;
extern long bytesread;

void dump_flash_to_file(void);

#endif

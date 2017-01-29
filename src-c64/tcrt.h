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


   tcrt.h: TCRT read/write

*/

#ifndef TCRT_H
#define TCRT_H

#define TCRT_VERSION            1
#define TCRT_HEADER_SIZE        16
#define TCRT_OFFSET_VERSION     16
#define TCRT_OFFSET_DATAADDR    18
#define TCRT_OFFSET_DATALENGTH  20
#define TCRT_OFFSET_CALLADDR    22
#define TCRT_OFFSET_FILENAME    24
#define TCRT_OFFSET_FLAGS       40
#define TCRT_FLAG_LOADERPRESENT 1
#define TCRT_OFFSET_LOADER      41
#define TCRT_OFFSET_FLASHLENGTH 212
#define TCRT_OFFSET_FLASHDATA   216

extern const uint8_t tcrt_header[TCRT_HEADER_SIZE];

void write_tcrt(void);
void dump_tcrt(void);

#endif

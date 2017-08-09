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


   io.c: wrappr code for cbm_* functions for accesing reu and normal cbm devices
         treating the reu content as single file where the 1st four bytes containing
	 the file size
*/



#include "io.h"
#include <conio.h>
#include <stdio.h>
#include <string.h>

signed char       reuLfn = -1;
unsigned long int reuSize;
unsigned long int reufilepos;


/* Copy 'size' bytes from REU memory starting at page 'page' offset 'offset' to 'dst' */
void transferFromReu(short int page, char offset, unsigned short int size, void * dst)
{
   *(unsigned short *) (0xdf02) = (unsigned short) dst;
   *(unsigned char *) (0xdf04)  = offset;
   *(unsigned short *) (0xdf05) = page;
   *(unsigned short *) (0xdf07) = size;
   *(unsigned char *) (0xdf09)  = 0;
   *(unsigned char *) (0xdf0a)  = 0;
   *(unsigned char *) (0xdf01)  = 253;
}

/* cbm_open wrapper with reu support, use "r:" as filename to access reu */
unsigned char __fastcall__ tc_cbm_open(unsigned char lfn, unsigned char device, unsigned char sec_addr, const char * name)
{
   if ( !strcmp(name, "r:"))
   {
      if (reuLfn != -1)
      {
         return 1;
      }

      reuLfn     = lfn;
      reufilepos = 0;

      transferFromReu(0, 0, 4, &reuSize);

      return 0;
   }
   else
   {
      return cbm_open(lfn, device, sec_addr, name);
   }
}

/* cbm_close wrapper with reu support */
void __fastcall__ tc_cbm_close(unsigned char lfn)
{
   if (lfn == reuLfn)
   {
      reuLfn = -1;
   }
   else
   {
      cbm_close(lfn);
   }
}

/* cbm_read wrapper with reu support */
int __fastcall__ tc_cbm_read(unsigned char lfn, void * buffer, unsigned int size)
{
   unsigned long int page;
   unsigned short    idx;

   if (lfn != reuLfn)
   {
      return cbm_read(lfn, buffer, size);
   }

   page = (reufilepos + 4) >> 8;
   idx  = (reufilepos + 4) & 0xff;

   if (size > reuSize - reufilepos)
   {
      size = reuSize - reufilepos;
   }

   if ( !size)
   {
      return 0;
   }

   transferFromReu(page, idx, size, buffer);

   reufilepos += size;

   return size;
}

/* cbm_write with reu support - without reu write support */
int __fastcall__ tc_cbm_write(unsigned char lfn, const void * buffer, unsigned int size)
{
   if (lfn != reuLfn)
   {
      return cbm_write(lfn, buffer, size);
   }

   return 0;
}

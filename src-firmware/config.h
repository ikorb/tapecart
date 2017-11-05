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


   config.h: static configuration

*/

#ifndef CONFIG_H
#define CONFIG_H

#include "autoconf.h"

/* I2C tapecart (early proto) */
#if CONFIG_HARDWARE_VARIANT == 1

#  define HAVE_I2C
#  define MEMTYPE_STRING "i2ceeprom"


/* AT45-SPI tapecart-xl (late proto) */
#elif CONFIG_HARDWARE_VARIANT == 2

#  define HAVE_AT45
#  define MEMTYPE_STRING "at45flash"


/* W25Q-SPI tapecart (release) */
#elif CONFIG_HARDWARE_VARIANT == 3

#  define HAVE_W25Q
#  define MEMTYPE_STRING "w25qflash"


/* W25Q-SPI tapecart-diy */
#elif CONFIG_HARDWARE_VARIANT == 4

#  define HAVE_W25Q
#  define MEMTYPE_STRING "w25qflash"


#else
#  error "Unknown hardware variant selected"
#endif


/* end of user-configurable options */

#include "arch-config.h"

#endif

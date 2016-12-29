/* tapecart - a tape port storage pod for the C64

   Copyright (C) 2013-2016  Ingo Korb <ingo@akana.de>
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


   bitbanging.c: Toggling and reading bits at just the right time

*/

#include "config.h"
#include "samd09c13a.h"
#include "system.h"
#include "timer.h"
#include "bitbanging.h"

// FIXME: consider updating with more precise timing
// (capture start via event, send via ISR or wavegen)

#define TICKS_PER_US 48

static void prepare_sendbyte(void) {
  disable_interrupts();
  TC1->COUNT16.COUNT.reg = 1.5 * TICKS_PER_US; // compensate for overhead
}

static void cleanup_sendbyte(void)  {
  TC1->COUNT16.CTRLBSET.reg =
    TC_CTRLBSET_CMD_STOP;
  enable_interrupts();
}

static void sendbyte_common(uint8_t byte) {
  /* wait until write is high or abort if motor is active */
  while (!get_write()) {
    if (get_motor())
      return;
  }

  TC1->COUNT16.CTRLBSET.reg = TC_CTRLBSET_CMD_RETRIGGER;

  /* at 4us: set write to output, send bits 5+4 on sense+write */
  while (TC1->COUNT16.COUNT.reg < 4 * TICKS_PER_US) ;
  set_write_output(true);
  set_sense(byte & (1 << 5));
  set_write(byte & (1 << 4));

  /* at 13us: send bits 7+6 on sense+write */
  while (TC1->COUNT16.COUNT.reg < 13 * TICKS_PER_US) ;
  set_sense(byte & (1 << 7));
  set_write(byte & (1 << 6));

  /* at 22us: send bits 1+0 on sense+write */
  while (TC1->COUNT16.COUNT.reg < 22 * TICKS_PER_US) ;
  set_sense(byte & (1 << 1));
  set_write(byte & (1 << 0));

  /* at 31us: send bits 3+2 on sense+write */
  while (TC1->COUNT16.COUNT.reg < 31 * TICKS_PER_US) ;
  set_sense(byte & (1 << 3));
  set_write(byte & (1 << 2));

  /* at 41us: set write to input again */
  while (TC1->COUNT16.COUNT.reg < 41 * TICKS_PER_US) ;
  set_write_output(false);
  set_write(true); // enables pullup

  /* wait until write is low to ensure we don't accidentally re-trigger immediately */
  delay_us(1);
  while (get_write() && !get_motor()) ;
}

void fast_sendbyte_loader(uint8_t b) {
  prepare_sendbyte();

  set_sense(false);
  sendbyte_common(b);
  set_sense(true);

  cleanup_sendbyte();
}

void fast_sendbyte_cmdmode(uint8_t b) {
  prepare_sendbyte();
  
  set_sense(true);
  sendbyte_common(b);
  set_sense(false);

  cleanup_sendbyte();
}

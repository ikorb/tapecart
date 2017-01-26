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


   system.c: Chip-specific system initialisation

*/

#include "config.h"
#include "arch-eeprom.h"
#include "samd09c13a.h"
#include "system.h"

static void setup_clocks(void) {
  /* set GCLK1 to internal 1MHz from OSC8M */
  /* (has its own prescaler, default /8)   */
  GCLK->GENCTRL.reg =
    GCLK_GENCTRL_ID(1)     |
    GCLK_GENCTRL_SRC_OSC8M |
    GCLK_GENCTRL_OE        |
    GCLK_GENCTRL_GENEN;
  GCLK->GENDIV.reg = GCLK_GENDIV_ID(1) | GCLK_GENDIV_DIV(1);

  /* connect FDPLL96M input to GCLK1 */
  GCLK->CLKCTRL.reg =
    GCLK_CLKCTRL_ID_FDPLL  |
    GCLK_CLKCTRL_GEN_GCLK1 |
    GCLK_CLKCTRL_CLKEN;

  /* connect FDPLL96M aux clock to 32kHz (GCLK2) */
  GCLK->CLKCTRL.reg =
    GCLK_CLKCTRL_ID_FDPLL32K |
    GCLK_CLKCTRL_GEN_GCLK2   |
    GCLK_CLKCTRL_CLKEN;

  /* configure FDPLL96M for x48 */
  SYSCTRL->DPLLRATIO.reg = SYSCTRL_DPLLRATIO_LDR(47);
  SYSCTRL->DPLLCTRLB.reg = SYSCTRL_DPLLCTRLB_REFCLK_GCLK;
  SYSCTRL->DPLLCTRLA.reg = SYSCTRL_DPLLCTRLA_ENABLE;

  while (!SYSCTRL->DPLLSTATUS.bit.ENABLE) ;
  while (!SYSCTRL->DPLLSTATUS.bit.LOCK)   ;

  /* enable one waitstate for flash access */
  NVMCTRL->CTRLB.bit.RWS = 1;

  /* switch main clock to FDPLL output, undivided */
  GCLK->GENCTRL.reg =
    GCLK_GENCTRL_ID(0)     |
    GCLK_GENCTRL_SRC_FDPLL |
    GCLK_GENCTRL_OE        |
    GCLK_GENCTRL_GENEN;

  /*** set up other clocks ***/
  /* use GCLK0 (CPU) for SERCOM0 core clock */
  GCLK->CLKCTRL.reg =
    GCLK_CLKCTRL_ID_SERCOM0_CORE |
    GCLK_CLKCTRL_GEN_GCLK0       |
    GCLK_CLKCTRL_CLKEN;

  /* use GCLK0 (CPU) for SERCOM1 core clock */
  GCLK->CLKCTRL.reg =
    GCLK_CLKCTRL_ID_SERCOM1_CORE |
    GCLK_CLKCTRL_GEN_GCLK0       |
    GCLK_CLKCTRL_CLKEN;

  /* use GCLK0 (CPU) for TC1+2 */
  GCLK->CLKCTRL.reg =
    GCLK_CLKCTRL_ID_TC1_TC2 |
    GCLK_CLKCTRL_GEN_GCLK0  |
    GCLK_CLKCTRL_CLKEN;
  
  /* set GCLK3 to FDPLL output divided by 4 (12 MHz) */
  /* (results in better glitch filtering)            */
  GCLK->GENCTRL.reg =
    GCLK_GENCTRL_ID(3)     |
    GCLK_GENCTRL_SRC_FDPLL |
    GCLK_GENCTRL_OE        |
    GCLK_GENCTRL_GENEN;

  GCLK->GENDIV.reg =
    GCLK_GENDIV_ID(3)  |
    GCLK_GENDIV_DIV(4);

  /* use GCLK3 for EIC */
  GCLK->CLKCTRL.reg =
    GCLK_CLKCTRL_ID_EIC    |
    GCLK_CLKCTRL_GEN_GCLK3 |
    GCLK_CLKCTRL_CLKEN;

  /* turn on bus clock to SERCOM0/1, TC1/2 */
  PM->APBCMASK.reg =
    PM_APBCMASK_SERCOM0 |
    PM_APBCMASK_SERCOM1 |
    PM_APBCMASK_TC1     |
    PM_APBCMASK_TC2;
}

void enable_interrupts(void) {
  __enable_irq();
}

void disable_interrupts(void) {
  __disable_irq();
}

void system_init(void) {
  /* enable BOD33 at ~3.0V, continuous mode */
  SYSCTRL->BOD33.reg =
    SYSCTRL_BOD33_LEVEL(44)    |
    SYSCTRL_BOD33_RUNSTDBY     |
    SYSCTRL_BOD33_ACTION_RESET |
    SYSCTRL_BOD33_HYST;
  SYSCTRL->BOD33.bit.ENABLE = 1;

  /* raise clock speed to 48MHz */
  setup_clocks();

  /* set up systick for use as delay timer */
  SysTick->CTRL = 0;
  SysTick->LOAD = (1<<24) - 1;

  /* prepare watchdog timer */
  WDT->CTRL.reg     = 0;
  WDT->CONFIG.reg   = WDT_CONFIG_PER_16K;
  WDT->INTENCLR.reg = WDT_INTENCLR_EW;
  WDT->CLEAR.reg    = WDT_CLEAR_CLEAR_KEY;
  //WDT->CTRL.reg     = WDT_CTRL_ENABLE;

  /* prepare TC1 as timing reference for the 2-bit-mode */
  TC1->COUNT16.CTRLA.reg    = TC_CTRLA_MODE_COUNT16;
  TC1->COUNT16.CTRLBSET.reg = TC_CTRLBSET_ONESHOT;
  TC1->COUNT16.CTRLA.reg    = TC_CTRLA_MODE_COUNT16 | TC_CTRLA_ENABLE;
  
  /* init eeprom emulation */
  eeprom_emu_init();
}

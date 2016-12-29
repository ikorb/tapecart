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


   timer.c: Chip-specific, timer-related code

*/

#include "config.h"
#include "samd09c13a.h"
#include "timer.h"

void RTC_Handler(void) {
  Pulsetimer_Handler();
  RTC->MODE1.INTFLAG.reg = RTC_MODE1_INTFLAG_OVF;
}

void delay_us(unsigned int time) {
  SysTick->LOAD = F_CPU / 1000000 * time - 1;
  SysTick->VAL  = 0;
  (void)SysTick->CTRL; // clear COUNTFLAG
  SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;

  while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)) ;

  SysTick->CTRL = 0;
}

void delay_ms(unsigned int time) {
  uint32_t delaytime = F_CPU / 1000 * time;

  while (delaytime > 0) {
    uint32_t curdelay = delaytime;

    if (curdelay >= (1 << 24))
      curdelay = (1 << 24) ;

    SysTick->LOAD = curdelay - 1;
    SysTick->VAL  = 0;
    (void)SysTick->CTRL; // clear COUNTFLAG
    SysTick->CTRL = /*SysTick_CTRL_CLKSOURCE_Msk |*/ SysTick_CTRL_ENABLE_Msk;
    __DSB();
    while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)) ;

    delaytime -= curdelay;
  }

  SysTick->CTRL = 0;
}

void pulsetimer_set_pulselen(uint8_t len) {
  /* Note: This assumes that the pulselen is set shortly after an overflow */
  RTC->MODE1.PER.reg = 3 * len;
}

void pulsetimer_enable(bool state) {
  if (state)
    // FIXME: Figure out why turning off enable doesn't help
    //RTC->MODE1.CTRL.bit.ENABLE = 1;
    NVIC_EnableIRQ(RTC_IRQn);
  else
    //RTC->MODE1.CTRL.bit.ENABLE = 0;
    NVIC_DisableIRQ(RTC_IRQn);
}

void pulsetimer_init(void) {
  /* send CPU clock to RTC */
  GCLK->CLKCTRL.reg =
    GCLK_CLKCTRL_ID_RTC    |
    GCLK_CLKCTRL_GEN_GCLK0 |
    GCLK_CLKCTRL_CLKEN;

  /* reset RTC */
  RTC->MODE1.CTRL.reg = RTC_MODE1_CTRL_SWRST;
  while ((RTC->MODE1.CTRL.reg & RTC_MODE1_CTRL_SWRST) ||
         (RTC->MODE1.STATUS.reg & RTC_STATUS_SYNCBUSY)) ;
  
  /* configure RTC to count at 3*125kHz (48 MHz / 128) */
  RTC->MODE1.CTRL.reg =
    RTC_MODE1_CTRL_MODE_COUNT16 |
    RTC_MODE1_CTRL_PRESCALER_DIV128;
  RTC->MODE1.INTFLAG.reg  = RTC_MODE1_INTFLAG_OVF;
  RTC->MODE1.INTENCLR.reg = RTC_MODE1_INTENCLR_MASK;
  RTC->MODE1.INTENSET.reg = RTC_MODE1_INTENSET_OVF;
  RTC->MODE1.COUNT.reg    = 0;
  RTC->MODE1.PER.reg      = 3 * 0xff;
  RTC->MODE1.CTRL.bit.ENABLE = 1;

  //NVIC_EnableIRQ(RTC_IRQn);
}

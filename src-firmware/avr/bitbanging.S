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


   bitbanging.S: fast synchronous 2-bit data transfer to the C64

*/

#include <avr/io.h>
#include "asmconfig.h"

        .section .text

        ;; ===================================================================
        ;;  Utility routines
        ;; ===================================================================

        ;;  Used by the macro below, don't call directly
cycleloop2:
        nop                     ; 1
cycleloop1:
        nop                     ; 1
cycleloop0:
        dec     r18             ; 1
        brne    cycleloop0      ; 2/1
        ret                     ; 4

        ;; This macro waits for the specified number of cycles
        ;;  Uses r18
        .macro delay_cycles num
        ldi     r18, 1 + (\num - 10) / 3 ; 1
        .if (\num - 10) % 3 == 0
        rcall   cycleloop0      ; 3
        .elseif (\num - 10) % 3 == 1
        rcall   cycleloop1      ; 3
        .else
        rcall   cycleloop2      ; 3
        .endif
        .endm


        ;; ===================================================================
        ;;  Fast 2-bit transmission
        ;; ===================================================================

        .global fast_sendbyte_loader
        ;; called with byte in r24, no return value
fast_sendbyte_loader:
        cli

        ;; send start signal to C64 (sense low)
        sbi     _SFR_IO_ADDR(SENSE_DDR), SENSE_BIT

        rcall   fast_sendbyte

        ;; set sense to input (high with ext. pullup) to signal busy
        cbi     _SFR_IO_ADDR(SENSE_DDR),  SENSE_BIT
        cbi     _SFR_IO_ADDR(SENSE_PORT), SENSE_BIT

        sei
        ret



        .global fast_sendbyte_cmdmode
        ;; called with byte in r24, no return value
fast_sendbyte_cmdmode:
        cli

        ;; send start signal to C64 (sense high)
        sbi     _SFR_IO_ADDR(SENSE_PORT), SENSE_BIT
        sbi     _SFR_IO_ADDR(SENSE_DDR),  SENSE_BIT

        rcall   fast_sendbyte

        ;; set sense low to signal busy
        cbi     _SFR_IO_ADDR(SENSE_PORT), SENSE_BIT
        sbi     _SFR_IO_ADDR(SENSE_DDR),  SENSE_BIT

        sei
        ret


        ;; common subroutine
fast_sendbyte:
        ;; wait until write is high or abort if motor is active
1:
#ifdef MOTOR_INVERTED
        sbis    _SFR_IO_ADDR(MOTOR_PIN), MOTOR_BIT
#else
        sbic    _SFR_IO_ADDR(MOTOR_PIN), MOTOR_BIT
#endif
        rjmp    exit
        
        sbis    _SFR_IO_ADDR(WRITE_PIN), WRITE_BIT
        rjmp    1b

        ;; delay - results in a setup time of 4us, port switches ~2us(?) after C64 sets it as input (FIXME: Check)
        delay_cycles 4*8 - 8

        ;; transmit bits 5+4
        in      r25, _SFR_IO_ADDR(WRITE_PORT)      ; 1
        bst     r24, 5                             ; 1 - read bit 5
        bld     r25, SENSE_BIT                     ; 1
        sbi     _SFR_IO_ADDR(WRITE_DDR), WRITE_BIT ; 2 - set write to output
        bst     r24, 4                             ; 1 - read bit 4
        bld     r25, WRITE_BIT                     ; 1
        out     _SFR_IO_ADDR(WRITE_PORT), r25      ; 1 - transmit

        ;; delay
        delay_cycles 9*8 - 5

        ;; transmit bits 7+6
        bst     r24, 7                             ; 1 - read bit 7
        bld     r25, SENSE_BIT                     ; 1
        bst     r24, 6                             ; 1 - read bit 6
        bld     r25, WRITE_BIT                     ; 1
        out     _SFR_IO_ADDR(WRITE_PORT), r25      ; 1 - transmit

        ;; delay
        delay_cycles 9*8 - 5

        ;; transmit bits 1+0
        bst     r24, 1                             ; 1 - read bit 1
        bld     r25, SENSE_BIT                     ; 1
        bst     r24, 0                             ; 1 - read bit 0
        bld     r25, WRITE_BIT                     ; 1
        out     _SFR_IO_ADDR(WRITE_PORT), r25      ; 1 - transmit

        ;; delay
        delay_cycles 9*8 - 5

        ;; transmit bits 3+2
        bst     r24, 3                             ; 1 - read bit 3
        bld     r25, SENSE_BIT                     ; 1
        bst     r24, 2                             ; 1 - read bit 2
        bld     r25, WRITE_BIT                     ; 1
        out     _SFR_IO_ADDR(WRITE_PORT), r25      ; 1 - transmit

        ;; hold time
        delay_cycles 10 * 8

exit:
        ;; restore write to input
        cbi     _SFR_IO_ADDR(WRITE_DDR),  WRITE_BIT
        cbi     _SFR_IO_ADDR(WRITE_PORT), WRITE_BIT

        delay_cycles 8

        ;; wait until write is low to ensure we're not faster than the C64
2:
#ifdef MOTOR_INVERTED
        sbis    _SFR_IO_ADDR(MOTOR_PIN), MOTOR_BIT
#else
        sbic    _SFR_IO_ADDR(MOTOR_PIN), MOTOR_BIT
#endif
        ret

        sbic    _SFR_IO_ADDR(WRITE_PIN), WRITE_BIT
        rjmp    2b
        ret

        .end

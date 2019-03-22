;;   tapecart - a tape port storage pod for the C64
;;
;;   Copyright (C) 2013-2017  Ingo Korb <ingo@akana.de>
;;   All rights reserved.
;;   Idea by enthusi
;;
;;   Redistribution and use in source and binary forms, with or without
;;   modification, are permitted provided that the following conditions
;;   are met:
;;   1. Redistributions of source code must retain the above copyright
;;      notice, this list of conditions and the following disclaimer.
;;   2. Redistributions in binary form must reproduce the above copyright
;;      notice, this list of conditions and the following disclaimer in the
;;      documentation and/or other materials provided with the distribution.
;;
;;   THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
;;   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
;;   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
;;   ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
;;   FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
;;   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
;;   OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
;;   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
;;   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
;;   OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
;;   SUCH DAMAGE.
;;
;;
;;   getbyte_fast.s: Fast byte read
;; 
;;   Note: This uses the opposite level on sense to determine ready-ness
;;         compared to the loader mode!
;;

        .export _tapecart_getbyte_fast
_tapecart_getbyte_fast: 
        ;; wait until tapecart is ready (sense high)
        lda #$10
rdyloop:        
        bit $01
        beq rdyloop

bad_line_chk:
        ;; check for bad lines
        lda $d011
        bmi no_bad_line

        lda $d012
        cmp #$30
        bcc no_bad_line

        cmp #$f8
        bcs no_bad_line
        
        and #$07
        cmp #$04                ; assume vertical raster scroll = 3
        bcs no_bad_line

        cmp #$02
        bcs bad_line_chk
no_bad_line:
        
        ;; send our own ready signal
        ldx #$38
        lda #$27
        stx $01                 ; set write high (start signal)
        sta $00                 ; 3 - switch write to input
        nop                     ; 2 - delay

        ;; receive byte
        lda $01                 ; 3 - read bits 5+4
        and #$18                ; 2 - mask
        lsr                     ; 2 - shift down
        lsr                     ; 2
        eor $01                 ; 3 - read bits 7+6 (EOR invertes 5+4!)
        lsr                     ; 2
        and #$0f                ; 2 - mask
        tax                     ; 2 - remember value

        lda $01                 ; 3 - read bits 1+0
        and #$18                ; 2 - mask
        lsr                     ; 2 - shift down
        lsr                     ; 2
        eor $01                 ; 3 - read bits 3+2 (EOR inverts 1+0!)
        lsr                     ; 2
        and #$0f                ; 2 - mask
        ora nibbletab,x         ; 4 - add upper nibble

        ldx #$2f                ; 2 - switch write to output
        stx $00                 ; 3
        ldx #$36                ; set write low again
        stx $01

        rts

nibbletab:
        .byt $00, $10, $20, $30, $40, $50, $60, $70
        .byt $80, $90, $a0, $b0, $c0, $d0, $e0, $f0

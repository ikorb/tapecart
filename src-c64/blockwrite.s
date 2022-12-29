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
;;   blockwrite.s: Send a block of bytes using 1-btit protocol

        .importzp sp
        .importzp sreg
        .import incsp2

        ;; tapecart_sendblock(void* address, uint8_t length_mod_256)
        .export _tapecart_sendblock
_tapecart_sendblock:
        sta chkend + 1          ; copy arguments to insn parameters
        ldy #$01
        lda (sp),y
        sta byteload + 2
        dey                     ; y=0
        lda (sp),y
        sta byteload + 1

        sei

byteloop:
        ;; wait until tapecart is ready (sense high)
        lda #$10
rdyloop:        
        bit $01
        beq rdyloop

        lda #$3f                ; switch sense to output
        sta $00

byteload:
        lda $0000,y             ; address selfmodded
        sta sreg                ; use sreg as temp storage
        ldx #$08

bitloop:
        asl sreg                ; grab next bit to send
        lda #$26                ; send bit, set sense low at the same time
        bcc bit_0
        lda #$36
bit_0:  sta $01

        ora #$08                ; set write high
        sta $01                 ; (this makes the tapecart capture the bit on sense)

        dex
        bne bitloop

        lda #$26                ; set write low again to finish last bit
        sta $01
        lda #$36                ; set sense high
        sta $01
        lda #$26                ; set sense low
        sta $01

        lda #$2f                ; make sense an input again
        sta $00
        nop                     ; safety delay to ensure tapecart has set its sense to output again

        iny
chkend: cpy #$00                ; length selfmodded
        bne byteloop

        lda #$01                ; ensure the kernal does not turn on the motor
        sta $c0

        cli

        jmp incsp2

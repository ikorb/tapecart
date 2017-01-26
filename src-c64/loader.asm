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
;;   loader.asm: Two-bit fastloader for the C64 tape port
;;


calladdr   = $aa
endaddr    = $ac
loadaddr   = $ae
info_block = calladdr           ; lowest address in the block
info_block_size = 6

	* = $0351
start:
        ror $d011               ; blank screen
        ;; handshake needs ~50ms, enough to ensure the screen is blank

        ;; send handshake
        sei
        clc                     ; fill the signature byte with a 0-bit
        ldy #16                 ; 16 bits
        ldx #$00
        stx $c6                 ; clear keyboard buffer

hskloop:
        lda #$27                ; motor off, write low
        rol handshk             ; read top bit of signature byte
        bcc hsk_0
        ora #$08                ; set write high
hsk_0:  sta $01

delay1: dex                     ; delay loop (motor line is heavily RC-filtered)
        nop
        bne delay1

        and #$df                ; motor on, keep write
        sta $01

delay2: dex
        bne delay2
        
        dey                     ; check if all bits sent
        bne hskloop

        ;; ensure a known state on write and turn off ROMs
        lda #$30                ; motor off, write low, all RAM
        sta $01

#if 0
        ;; wait until sense is 0
        ;; (not really needed, the handshake needs enough time)
sensechk:
        lda $01
        and #$10
        bne sensechk
#endif


        ;; read info block for load/start addr etc.
        ldy #256 - info_block_size
infoloop:
        jsr getbyte
        sta info_block - (256 - info_block_size),y
        iny
        bne infoloop

read    jsr getbyte             ; read byte
        sta (loadaddr),y
        
        inc loadaddr            ; update target address
        bne no_carry
        inc loadaddr+1

no_carry:
        lda loadaddr            ; check low byte for end
        cmp endaddr
        bne read

        ldx loadaddr+1          ; check high byte
        cpx endaddr+1
        bne read

        ldy #$37                ; turn on ROMs
        sty $01

        sec                     ; turn screen on
        rol $d011

        stx $2e                 ; update BASIC pointers
        stx $30                 ; (FIXME: Check if one of the ROM calls helps here)
        sta $2d
        sta $2f

        cli                     ; enable interrupts
        jsr $e453               ; restore vectors
        jmp (calladdr)


        ;; returns byte in A, preserves Y
getbyte:
        ;; wait until AVR is ready (sense low)
        lda #$10
rdyloop:        
        bit $01
        bne rdyloop

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
        eor $01                 ; 3 - read bits 7+6 (EOR inverts 5+4)
        lsr                     ; 2
        and #$0f                ; 2 - mask
        tax                     ; 2 - remember value

        lda $01                 ; 3 - read bits 1+0
        and #$18                ; 2 - mask
        lsr                     ; 2 - shift down
        lsr                     ; 2
        eor $01                 ; 3 - read bits 3+2 (EOR inverts 1+0)
        lsr                     ; 2
        and #$0f                ; 2 - mask
        ora nibbletab,x         ; 4 - add upper nibble

        ldx #$2f                ; 2 - switch write to output
        stx $00                 ; 3
        inx                     ; set write low again
        stx $01

        rts

nibbletab:
        .byt $00, $10, $20, $30, $40, $50, $60, $70
        .byt $80, $90, $a0, $b0, $c0, $d0, $e0, $f0
        
handshk:
        .byt $ca                ; results in $ca65 if used with CLC

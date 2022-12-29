;;   tapecart - a tape port storage pod for the C64
;;
;;   todinit.s: initialize TOD for correct tick frequency
;;
;;   Code directly stolen from https://codebase64.org/doku.php?id=base:efficient_tod_initialisation
        
        .export _init_timing
_init_timing:
        sei             ; accounting for NMIs is not needed when
        lda #$00        ; used as part of application initialisation
        sta $dd08       ; TO2TEN start TOD - in case it wasn't running
wait:   cmp $dd08       ; TO2TEN wait until tenths
        beq wait        ; register changes its value
 
        lda #$ff        ; count from $ffff (65535) down
        sta $dd04       ; TI2ALO both timer A register
        sta $dd05       ; TI2AHI set to $ff
 
        lda #%00010001  ; bit seven = 0 - 60Hz TOD mode
        sta $dd0e       ; CI2CRA start the timer
 
        lda $dd08       ; TO2TEN
wait2:  cmp $dd08       ; poll TO2TEN for change
        beq wait2
 
        lda $dd05       ; TI2AHI expect (approximate) $7f4a $70a6 $3251 $20c0
        cli
 
        cmp #$51        ; about the middle (average is $50c0)
        bcs ticks_60hz
        
        ; 50Hz on TOD pin
        lda #$88
        jmp exit
 
ticks_60hz:
        ; 60Hz on TOD pin
        lda #$08

exit:   sta $dd0e
        rts

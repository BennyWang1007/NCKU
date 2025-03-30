List p = 18f4520
    #include <p18f4520.inc>
        CONFIG OSC = INTIO67
        CONFIG WDT = OFF
        org 0x00
main:
;   GOTO basic_init
    GOTO advanced_init
;   GOTO bonus_init                                                         
        
        
basic_init:
    MOVLB 0x1 ; set BSR

    MOVLW 0x01 ; init [0x100]
    MOVWF 0x00, 1

    MOVLW 0x00 ; init [0x116]
    MOVWF 0x16, 1

    LFSR 0, 0x100 ; set pointer
    LFSR 1, 0x116

    GOTO basic_main

basic_main:
    MOVLW d'10' ; init counter to 10
    MOVWF 0x00
    MOVF INDF1, W
basic_loop:
    ; 10 times
    ADDWF POSTINC0, W
    MOVWF INDF0
    ADDWF POSTDEC1, W
    MOVWF INDF1

    DECF 0x00
    TSTFSZ 0x00
    GOTO basic_loop

    ADDWF POSTINC0, W
    MOVWF INDF0

    GOTO termination


advanced_init:
    LFSR 0, 0x100 ; set pointer
    MOVLW 0xFF
    MOVWF POSTINC0
    MOVLW 0xFE
    MOVWF POSTINC0
    MOVLW 0xFD
    MOVWF POSTINC0
    MOVLW 0xFC
    MOVWF POSTINC0
    MOVLW 0xFB
    MOVWF POSTINC0
    MOVLW 0xFA
    MOVWF POSTINC0
    MOVLW 0xF9
    MOVWF POSTINC0
    LFSR 0, 0x100 ; start pointer
    LFSR 1, 0x107 ; end pointer

    GOTO bubble_sort

bubble_sort:
    MOVFF FSR0L, FSR2L    ; set FSR2 to start
    MOVFF FSR0H, FSR2H

inner_loop:
    ; test if inner loop reached end
    MOVF POSTINC2, W    ; FSR2++

    MOVF FSR2L, W
    CPFSEQ  FSR1L
    GOTO inner_loop_not_end
    MOVF FSR2H, W
    CPFSEQ  FSR1H
    GOTO inner_loop_not_end
    GOTO inner_loop_reach_end

inner_loop_reach_end:
    DECF FSR1L, F      ; end pointer--
    BTFSS STATUS, 0    ; check borrow
    DECF FSR1H, F
    ; compare if FSR1 == FSR0 (start, end)
    MOVF FSR1L, W
    CPFSEQ  FSR0L
    GOTO bubble_sort
    MOVF FSR1H, W
    CPFSEQ FSR0H
    GOTO inner_loop
    GOTO termination


inner_loop_not_end:
    MOVF POSTDEC2, W    ; W = next
    CPFSLT INDF2        ; if cur < next
    GOTO swap
back:
    MOVF POSTINC2, W    ; FSR2++
    GOTO inner_loop

swap:
    MOVF POSTINC2, W    ; W = first
    MOVWF 0x01            ; [0x001] = first
    MOVF POSTDEC2, W    ; W = second
    MOVWF POSTINC2        ; first = second
    MOVF 0x01, W
    MOVWF INDF2            ; second = first
    MOVF POSTDEC2        ; FSR2--
    GOTO back


bonus_init:
    LFSR 0, 0x000 ; set pointer
    MOVLW 0x28
    MOVWF POSTINC0
    MOVLW 0x34
    MOVWF POSTINC0
    MOVLW 0x7A
    MOVWF POSTINC0
    MOVLW 0x80
    MOVWF POSTINC0
    MOVLW 0xA7
    MOVWF POSTINC0
    MOVLW 0xD1
    MOVWF POSTINC0
    MOVLW 0xFE
    MOVWF POSTINC0

    MOVLW 0xFF ; target
    MOVWF 0x10

    LFSR 0, 0x000 ; left
    LFSR 1, 0x006 ; right

    GOTO binary_saerch

binary_saerch:
    MOVF FSR1L, W
    CPFSGT FSR0L
    GOTO search_logic
    GOTO not_found

search_logic:
    ADDWF FSR0L, W
    RRNCF WREG
    ANDLW b'01111111'
    MOVWF 0x20 ; tmp for mid
    MOVWF FSR2L
    MOVF INDF2, W
    CPFSEQ 0x10
    GOTO not_found_yet
    GOTO found

not_found_yet:
    CPFSLT 0x10
    GOTO left_mid_add_one
    MOVF 0x20, W
    DECF WREG
    MOVWF FSR1L
    GOTO binary_saerch

left_mid_add_one:
    MOVF 0x20, W
    INCF WREG
    MOVWF FSR0L
    GOTO binary_saerch

found:
    MOVLW 0xFF
    MOVWF 0x11
    GOTO termination

not_found:
    MOVLW 0x00
    MOVWF 0x11
    GOTO termination

termination:
    NOP
end



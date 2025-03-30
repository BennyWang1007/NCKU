List p = 18f4520
    #include <p18f4520.inc>
        CONFIG OSC = INTIO67
        CONFIG WDT = OFF
        org 0x00
        
main:
;   GOTO basic_init
;   GOTO advanced_init
    GOTO bonus_init

basic_init:
    MOVLW 0x5F
    MOVWF TRISA
basic_main:
    ; mask for arithmetic shift right
    MOVLW 0x40
    MOVWF 0x00
    ; logical shift left
    MOVF TRISA, W
    RLNCF WREG
    ANDLW 0xFE
    MOVWF TRISA
    ; arithmetic shift right
    RRNCF TRISA
    MOVF TRISA, W
    ANDWF 0x00, W
    RLNCF WREG
    ADDWF TRISA, F
    GOTO termination

advanced_init:
    MOVLW 0x12
    MOVWF 0X00
    MOVLW 0xCB
    MOVWF 0X01
    MOVLW 0x09
    MOVWF 0X10
    MOVLW 0x35
    MOVWF 0X11
    CLRF 0x20
    CLRF 0x21
    CLRF 0x22
    CLRF 0x23
davanced_main:
    ; 0x01 * 0x11
    MOVF 0x01, W
    MULWF 0x11
    MOVFF PRODL, 0x23
    MOVFF PRODH, 0x22

    ; 0x01 * 0x10
    MULWF 0x10
    MOVF PRODL, W
    ADDWF 0x22, F
    MOVF PRODH, W
    ADDWFC 0x21, F

    ; 0x00 * 0x11
    MOVF 0x00, W
    MULWF 0x11
    MOVF PRODL, W
    ADDWF 0x22, F
    MOVF PRODH, W
    ADDWFC 0x21, F

    BTFSC STATUS, C ; if carry is set
    INCF 0x20, F

    ; 0x00 * 0x10
    MOVF 0x00, W
    MULWF 0x10
    MOVF PRODL, W
    ADDWF 0x21, F
    MOVF PRODH, W
    ADDWFC 0x20, F

    GOTO termination


bonus_init:
    MOVLW 0x2A
;   MOVLW 0x00
    MOVWF 0x00
    MOVLW 0x11
    MOVWF 0x01
    CLRF 0x02

    MOVLW 0x01 ; [0x21] = ONE
    MOVWF 0x21
    LFSR 0, 0x000 ; ptr to higher byte
    LFSR 1, 0x010 ; ptr to found
    LFSR 2, 0x011 ; ptr to result

bonus_main:
    CALL find_max_bit
    TSTFSZ INDF1            ; if 'found' is non-zero
    GOTO found_H
    LFSR 0, 0x001            ; switch ptr to low byte
    CALL find_max_bit
    TSTFSZ INDF1            ; if 'found' is non-zero
    GOTO found_L
    GOTO termination

found_H:
    MOVF INDF2, W            ; get result
    ADDLW 0x07                ; add 7
    MOVWF 0x02
    GOTO find_rest

found_L:
    MOVFF INDF2, 0x02        ; get result
    DECF 0x02                ; dec 1
    GOTO find_rest

find_rest:
    MOVF 0x12, W
    SUBWF INDF0, F
    TSTFSZ 0x00
    GOTO not_zeros
    TSTFSZ 0x01
    GOTO not_zeros
    MOVF 0x12, W ; restore [0x00]
    ADDWF INDF0, F
    GOTO termination

not_zeros:
    MOVF 0x12, W ; restore [0x00]
    ADDWF INDF0, F
    INCF 0x02
    GOTO termination

find_max_bit:
    CLRF INDF1 ; set if found
    MOVLW 0x09
    MOVWF 0x11 ; counter
    MOVFF 0x21, 0x12 ; mask = 1

find_max_bit_loop:
    DCFSNZ 0x11
    RETURN
    RRNCF 0x12
    MOVF 0x12, W
    ANDWF INDF0, W
    BTFSC STATUS, Z ; if zero, continue
    GOTO find_max_bit_loop
    MOVFF 0x21, INDF1
    RETURN

termination:
    NOP
end


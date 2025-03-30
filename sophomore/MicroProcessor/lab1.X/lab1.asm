List p = 18f4520
    #include <p18f4520.inc>
        CONFIG OSC = INTIO67
        CONFIG WDT = OFF
        org 0x00

main:
;    GOTO basic_init
;    GOTO advanced_init
    GOTO bonus_init

; ------------- BASIC -------------

basic_init:
    GOTO init_basic1

init_basic1:
    MOVLW 0x11
    MOVWF 0x00 ; init [0x000]
    MOVLW 0xA1
    MOVWF 0x01 ; init [0x001]
    MOVLW 0xC5
    MOVWF 0x10 ; init C
    MOVLW 0xB7
    MOVWF 0x11 ; init D
    GOTO basic

init_basic2:
    MOVLW 0x11
    MOVWF 0x00 ; init [0x000]
    MOVLW 0x12
    MOVWF 0x01 ; init [0x001]
    MOVLW 0x2A
    MOVWF 0x10 ; init C
    MOVLW 0x07
    MOVWF 0x11 ; init D
    GOTO basic

init_basic3:
    MOVLW 0x07
    MOVWF 0x00 ; init [0x000]
    MOVLW 0x09
    MOVWF 0x01 ; init [0x001]
    MOVLW 0x12
    MOVWF 0x10 ; init C
    MOVLW 0x01
    MOVWF 0x11 ; init D
    GOTO basic

basic:
    MOVF 0x00, W
    ADDWF 0x01, W   ; WREG = [0x000] + [0x001]
    MOVWF 0x02      ; A1 = WREG

    MOVF 0x11, W    ; load D
    SUBWF 0x10, 0   ; WREG = C - D
    MOVWF 0x12

    MOVF 0x12, W    ; load A2
    CPFSGT 0x02     ; compare A1 > A2
    GOTO less_or_equal
    MOVLW 0xAA
    MOVWF 0x20
    GOTO termination
less_or_equal:
    CPFSEQ 0x02     ; compare A1 == A2
    GOTO less_than
    MOVLW 0xBB
    MOVWF 0x20
    GOTO termination
less_than:
    MOVLW 0xCC
    MOVWF 0x20

; ------------- ADVANCED -------------

advanced_init:
    CLRF 0x02
    CLRF 0x03
    GOTO advanced_init2

advanced_init1:
    MOVLW b'01010101'
    MOVWF 0x00
    MOVLW b'01111101'
    MOVWF 0x01
    GOTO advanced_main

advanced_init2:
    MOVLW 0xF7
    MOVWF 0x00
    MOVLW 0X9F
    MOVWF 0x01
    GOTO advanced_main


advanced_main:
    MOVLW b'11110000'
    ANDWF 0x00, W   ; extract the first 4 bit
    MOVWF 0x02

    MOVLW b'00001111'
    ANDWF 0x01, W   ; extract the last 4 bit
    ADDWF 0x02, F

    ; get the number of 0 bits
    BTFSS 0x02, 0
    INCF 0x03
    BTFSS 0x02, 1
    INCF 0x03
    BTFSS 0x02, 2
    INCF 0x03
    BTFSS 0x02, 3
    INCF 0x03
    BTFSS 0x02, 4
    INCF 0x03
    BTFSS 0x02, 5
    INCF 0x03
    BTFSS 0x02, 6
    INCF 0x03
    BTFSS 0x02, 7
    INCF 0x03

    GOTO termination

; -------------- bonus --------------

bonus_init:
    MOVLW 0xAA
    MOVWF 0x00

    MOVLW 0x10
    MOVWF 0x10
    GOTO bonus_main


bonus_main:
    MOVF 0x00, W
    MOVWF 0x02 ; original number
    goto bonus_loop

bonus_loop:
    MOVF 0x00, W
    ANDLW 0x03
    MOVWF 0x01
    TSTFSZ 0x01 ;skip if result = 0
    GOTO not_four
    INCF 0x10
    INCF 0x10


loop_again:
    RRNCF 0x00
    MOVF 0x00, W
    CPFSEQ 0x02
    GOTO bonus_loop
    GOTO termination

not_four:
    MOVF 0x00, W
    ANDLW 0x01
    MOVWF 0x01
    TSTFSZ 0x01 ;skip if result = 0
    GOTO not_two
    INCF 0x10
    GOTO loop_again

not_two:
    DECF 0x10
    GOTO loop_again

termination:
    NOP
end

; -------------- inst. list ---------------
;    MOVLW 0x16 ; move literal to wreg
;    MOVWF 0x00 ; move wreg to f
;    INCF 0x00 ; increment f
;    ADDWF 0x01, F ; add wreg to f
;    CLRF 0x00 ; clear f
;    RRNCF 0x01 ; rotate right f (no carry)
;    RLNCF 0x01
;    DECFSZ 0x00 ; decrease file skip (next line if) zero
;    BTFSS 0x01, 4 ; bits test f skip if set
;    GOTO loop
;end






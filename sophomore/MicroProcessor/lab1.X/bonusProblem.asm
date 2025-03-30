List p = 18f4520
    #include <p18f4520.inc>
        CONFIG OSC = INTIO67
        CONFIG WDT = OFF
        org 0x00
	
main:
    GOTO bonus_init
    
bonus_init:
    MOVLW b'10000001'
    MOVWF 0x00

    MOVLW 0x05
    MOVWF 0x10
    GOTO bonus_main
    
    
bonus_main:
    MOVF 0x00, W
    MOVWF 0x02 ; store original number to [0x002]
    goto bonus_loop
    
bonus_loop:
    MOVF 0x00, W
    ANDLW 0x03
    MOVWF 0x01
    TSTFSZ 0x01 ; skip if not a multiple of 4
	GOTO not_four
    INCF 0x10
    INCF 0x10
    
loop_again:
    RRNCF 0x00
    MOVF 0x00, W
    CPFSEQ 0x02 ; skip if the the number == original one
	GOTO bonus_loop
    GOTO termination
    
not_four:
    MOVF 0x00, W
    ANDLW 0x01
    MOVWF 0x01
    TSTFSZ 0x01 ; skip if not a multiple of 2
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






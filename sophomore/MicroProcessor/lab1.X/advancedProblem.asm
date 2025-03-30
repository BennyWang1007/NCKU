List p = 18f4520
    #include <p18f4520.inc>
        CONFIG OSC = INTIO67
        CONFIG WDT = OFF
        org 0x00
	
main:
    GOTO advanced_init
	
    
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
    MOVLW 0xFF
    MOVWF 0x00
    MOVLW 0X1E
    MOVWF 0x01
    GOTO advanced_main
    
advanced_main:
    MOVLW b'11110000'
    ANDWF 0x00, W ; extract the first 4 bit
    MOVWF 0x02
    
    MOVLW b'00001111'
    ANDWF 0x01, W ; extract the last 4 bit
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
    MOVLW b'10000001'
    MOVWF 0x00
;    MOVWF 0x02

    MOVLW 0x05
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



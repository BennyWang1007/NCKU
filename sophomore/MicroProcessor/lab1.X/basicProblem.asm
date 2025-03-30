List p = 18f4520
    #include <p18f4520.inc>
        CONFIG OSC = INTIO67
        CONFIG WDT = OFF
        org 0x00
	
main:
    GOTO basic_init
	
; ------------- BASIC -------------

basic_init:
    GOTO basic_init3
    
basic_init1:
    MOVLW 0x01
    MOVWF 0x00 ; init [0x000]
    MOVLW 0x02
    MOVWF 0x01 ; init [0x001]
    MOVLW 0x04
    MOVWF 0x10 ; init C
    MOVLW 0x03
    MOVWF 0x11 ; init D
    GOTO basic

basic_init2:
    MOVLW 0x11
    MOVWF 0x00 ; init [0x000]
    MOVLW 0x12
    MOVWF 0x01 ; init [0x001]
    MOVLW 0x2A
    MOVWF 0x10 ; init C
    MOVLW 0x07
    MOVWF 0x11 ; init D
    GOTO basic

basic_init3:
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
    ADDWF 0x01, W ; WREG = [0x000] + [0x001]
    MOVWF 0x02 ; A1 = WREG
	
    MOVF 0x11, W ; load D
    SUBWF 0x10, 0 ; WREG = C - D
    MOVWF 0x12
    
    MOVF 0x12, W ; load A2
    CPFSGT 0x02 ; compare A1 > A2
	GOTO less_or_equal
    MOVLW 0xAA
    MOVWF 0x20
    GOTO termination
    
less_or_equal:
    CPFSEQ 0x02 ; compare A1 == A2
	GOTO less_than
    MOVLW 0xBB
    MOVWF 0x20
    GOTO termination
    
less_than:
    MOVLW 0xCC
    MOVWF 0x20
    
    
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



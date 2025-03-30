List p = 18f4520
    #include <p18f4520.inc>
        CONFIG OSC = INTIO67
        CONFIG WDT = OFF
        org 0x00
MOVLF macro literal, F
	MOVLW literal
	MOVWF F
endm
bonus:
	MOVLF 0x09, 0x10
	CALL fib
	MOVLF d'15', 0x10
	CALL fib
	GOTO termination
fib:
	CLRF 0x00
	CLRF 0x01
	MOVLF 0x00, 0x03
	MOVLF 0x01, 0x05
	TSTFSZ 0x10
	GOTO fib_loop
	RETURN				; case n = 0, return 0
fib_loop:
	MOVF 0x03, W		; f_n = f_n-1 + f_n-2
	ADDWF 0x05, W
	MOVWF 0x01
	MOVF 0x02, W
	ADDWFC 0x04, W
	MOVWF 0x00
	MOVFF 0x02, 0x04	; f_n-2 = f_n-1
	MOVFF 0x03, 0x05
	MOVFF 0x00, 0x02	; f_n-1 = f_n
	MOVFF 0x01, 0x03
	DCFSNZ 0x10
	RETURN
	GOTO fib_loop
termination:
end
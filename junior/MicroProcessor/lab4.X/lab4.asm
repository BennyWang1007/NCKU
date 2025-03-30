List p = 18f4520
    #include <p18f4520.inc>
        CONFIG OSC = INTIO67
        CONFIG WDT = OFF
        org 0x00
        
main:
    RCALL basic_main
;   RCALL advanced_main
    RCALL bonus_main
    GOTO termination
        
MOVLF macro literal, F
    MOVLW literal
    MOVWF F
endm

; dst = src1 - src2
SUB2Byte macro src1H, src1L, src2H, src2L, dstH, dstL
    ; xl - yl
    MOVF src2L, W
    SUBWF src1L, W
    MOVWF dstL
    ; xh - yh - BORROW
    MOVF src2H, W
    SUBWFB src1H, W
    MOVWF dstH
endm

Sub_Mul macro xh, xl, yh, yl
    MOVLF xh, 0x20
    MOVLF xl, 0x21
    MOVLF yh, 0x22
    MOVLF yl, 0x23
    SUB2Byte 0x20, 0x21, 0x22, 0x23, 0x00, 0x01

    MOVF 0x00, W
    MULWF 0x01
    MOVFF PRODH, 0x10
    MOVFF PRODL, 0x11
endm

ABSubCD macro a, b, c, d, dstH, dstL
    ; a * b
    MOVF a, W
    MULWF b
    MOVFF PRODH, 0x10
    MOVFF PRODL, 0x11
    ; c * d
    MOVF c, W
    MULWF d
    ; a * b - c * d
    SUB2Byte 0x10, 0x11, PRODH, PRODL, dstH, dstL
endm

basic_main:
;   Sub_Mul 0x0A, 0x04, 0x04, 0x02
    Sub_Mul 0x03, 0xA5, 0x02, 0xA7
    Sub_Mul 0x02, 0x0C, 0x00, 0x0F
    RETURN

advanced_init macro a1, a2, a3, b1, b2, b3
    MOVLF a1, 0x00
    MOVLF a2, 0x01
    MOVLF a3, 0x02
    MOVLF b1, 0x03
    MOVLF b2, 0x04
    MOVLF b3, 0x05
endm

advanced_main:
;   advanced_init 0x0B, 0x00, 0x10, 0x0C, 0x00, 0x06
    advanced_init 0x4, 0x2, 0x4, 0x3, 0x2, 0x6
    RCALL cross
    advanced_init 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
    RCALL cross
    RETURN

cross:
    ABSubCD 0x01, 0x05, 0x02, 0x04, 0x30, 0x20    ; a2 * b3 - a3 * b2
    ABSubCD 0x02, 0x03, 0x00, 0x05, 0x30, 0x21    ; a3 * b1 - a1 * b3
    ABSubCD 0x00, 0x04, 0x01, 0x03, 0x30, 0x22    ; a1 * b2 - a2 * b1
    CLRF 0x30
    RETURN

bonus_main:
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
    RETURN                ; case n = 0, return 0
fib_loop:
    MOVF 0x03, W        ; f_n = f_n-1 + f_n-2
    ADDWF 0x05, W
    MOVWF 0x01
    MOVF 0x02, W
    ADDWFC 0x04, W
    MOVWF 0x00
    MOVFF 0x02, 0x04    ; f_n-2 = f_n-1
    MOVFF 0x03, 0x05
    MOVFF 0x00, 0x02    ; f_n-1 = f_n
    MOVFF 0x01, 0x03
    DCFSNZ 0x10
    RETURN
    GOTO fib_loop

termination:
    NOP
end



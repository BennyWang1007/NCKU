#include "xc.inc"

GLOBAL _gcd
PSECT mytext, local, class = CODE, reloc = 2

; dst = src1 - src2
SUB2Byte macro src1L, src1H, src2L, src2H, dstL, dstH
    MOVF src2L, W       ; xl - yl
    SUBWF src1L, W
    MOVWF dstL
    MOVF src2H, W       ; xh - yh - BORROW
    SUBWFB src1H, W
    MOVWF dstH
endm

; result = scr1 > scr2 ? 1 : 0
CMP2Byte macro src1L, src1H, src2L, src2H, result
    ; compare high bytes
    MOVF src2H, W
    SUBWF src1H, W          ; W = src2H - src1H
    BTFSS STATUS, 2         ; skip if src1H == src2H
    BRA _cmp                ; if high bytes not equal
    ; compare low bytes
    MOVF src2L, W
    SUBWF src1L, W          ; W = src2L - src1L
    BTFSS STATUS, 2         ; skip if src1L == src2L
    BRA _cmp
    BRA _src2_greater_equal
_cmp:
    BTFSC   STATUS, 0        ; check if src1 >= src2
    BRA    _src1_greater
_src2_greater_equal:
    MOVLW   0            ; src1 <= src2
    MOVWF   result
    BRA _done
_src1_greater:
    MOVLW   1            ; src1 > src2
    MOVWF   result
_done:
endm

SWAP2Byte macro src1L, src1H, src2L, src2H, TEMP
    ; swap low bytes
    MOVF   src1L, W
    MOVWF   TEMP         ; TEMP = src1L
    MOVF   src2L, W
    MOVWF   src1L        ; src1L = src2L
    MOVF   TEMP, W
    MOVWF   src2L        ; src2L = TEMP
    ; swap high bytes
    MOVF   src1H, W
    MOVWF   TEMP         ; TEMP = src1H
    MOVF   src2H, W
    MOVWF   src1H        ; src1H = src2H
    MOVF   TEMP, W
    MOVWF   src2H        ; src2H = TEMP
endm

_gcd:
    CMP2Byte 0x03, 0x04, 0x01, 0x02, 0x07 ; 0x07 = (0x03 > 0x01)
    TSTFSZ 0x07
    RCALL _swap        ; make 0x01 > 0x03
    SUB2Byte 0x01, 0x02, 0x03, 0x04, 0x01, 0x02
    TSTFSZ 0x01
    GOTO _gcd        ; if not zero, loop again
;    GOTO _low_zero
_low_zero:
    TSTFSZ 0x02
    GOTO _gcd        ; if not zero, loop again
;    GOTO _zeros
_zeros:
    MOVFF 0x03, 0x01
    MOVFF 0x04, 0x02
    RETURN
_swap:
    SWAP2Byte 0x01, 0x02, 0x03, 0x04, 0x08
    RETURN

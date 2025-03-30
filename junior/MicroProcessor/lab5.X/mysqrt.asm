#include "xc.inc"

GLOBAL _mysqrt
PSECT mytext, local, class = CODE, reloc = 2

_mysqrt:
    MOVFF 0x001, LATD
    MOVLW 0x00
    MOVWF LATC
_mysqrt_loop:
    MOVF LATC, W
    MULWF LATC
    TSTFSZ PRODH
    GOTO _mysqrt_ret    ; case result = 16
    MOVF PRODL, W
    CPFSGT LATD
    GOTO _mysqrt_ret
    INCF LATC
    GOTO _mysqrt_loop
_mysqrt_ret:
    MOVF LATC, W
    RETURN

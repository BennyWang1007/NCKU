#include "xc.inc"

GLOBAL _multi_signed
PSECT mytext, local, class = CODE, reloc = 2

#define A 0x00
#define B 0x01

#define A_NEG 0x03
#define B_NEG 0x04
#define COUNTER 0x05

#define RES_L 0x11
#define RES_H 0x10

#define RET_L 0x01
#define RET_H 0x02

#define CARRY 0
#define ZERO 2


_multi_signed:
    CLRF A_NEG
    CLRF B_NEG
    CLRF RES_L
    CLRF RES_H
    MOVWF A         ; mov a to 0x02
    BTFSC A, 7      ; if a is neg
    GOTO _neg_a
    GOTO _check_b
_neg_a:
    NEGF A
    INCF A_NEG
_check_b:
    BTFSC B, 3
    GOTO _neg_b
    GOTO _multiply_ab_init
_neg_b:
    NEGF B
    INCF B_NEG
_multiply_ab_init:
    MOVLW 0x08
    MOVWF COUNTER
_multiply_ab:
    BCF STATUS, CARRY     ; left shift result
    RLCF RES_L
    RLCF RES_H
    BTFSC A, 7            ; if bit is set, result += b
    GOTO _add_b_to_res
    GOTO _next
_add_b_to_res:
    MOVF B, W
    ADDWF RES_L, F
    BTFSC STATUS, CARRY   ; if carry
    INCF RES_H
_next:
;   CLRF STATUS
;   RLCF A
    RLNCF A
    DECFSZ COUNTER
    GOTO _multiply_ab
_finalize:
    MOVF  A_NEG, W
    SUBWF B_NEG, W
    MOVFF RES_H, 0x02
    MOVFF RES_L, 0x01
    BTFSS STATUS, ZERO
    GOTO _complement_res    ; if sign is different
    RETURN
_complement_res:
    COMF RET_L
    COMF RET_H
    INCF RET_L
    BtFSC STATUS, CARRY
    INCF RET_H
    RETURN

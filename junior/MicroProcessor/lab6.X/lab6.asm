
#include <p18f4520.inc>
    LIST p = 18f4520
    CONFIG OSC = INTIO67 ; Set internal oscillator to 1 MHz
    CONFIG WDT = OFF     ; Disable Watchdog Timer
    CONFIG LVP = OFF     ; Disable Low Voltage Programming

L1  EQU 0x14         ; Define L1 memory location
L2  EQU 0x15         ; Define L2 memory location
    org 0x00            ; Set program start address to 0x00
main:
;   RCALL basic
;   RCALL advanced
    RCALL bonus

; delay = (2+(5+8*num1)*num2) * 4 ?s
DELAY_WITH_CHECK macro num1, num2, label
    local LOOP1, LOOP2
    MOVLW num2
    MOVWF L2
LOOP2:
    MOVLW num1
    MOVWF L1
LOOP1:
    btn_branch_pressed label ; 3 cycle
    NOP
    NOP
    DECFSZ L1, 1
    BRA LOOP1           ; total 8 cycles
    DECFSZ L2, 1        ; 3 cycles
    BRA LOOP2
    endm

; delay = (2+(5+8*num1)*num2) * 4 ?s
DELAY macro num1, num2
    local LOOP1, LOOP2
    MOVLW num2
    MOVWF L2
LOOP2:
    MOVLW num1
    MOVWF L1
LOOP1:
    NOP
    NOP
    NOP
    NOP
    NOP
    DECFSZ L1, 1
    BRA LOOP1           ; total 8 cycles
    DECFSZ L2, 1        ; 3 cycles
    BRA LOOP2
    endm

init:
; let pin can receive digital signal
    MOVLW 0x0f          ; Set ADCON1 register for digital mode
    MOVWF ADCON1        ; Store WREG value into ADCON1 register
    CLRF PORTB          ; Clear PORTB
    BSF TRISB, 0        ; Set RB0 as input  (TRISB = xxxx xxx1)
    CLRF LATA           ; Clear LATA
    BCF TRISA, 0        ; Set RA0 as output (TRISA = xxxx xxx0)
    BCF TRISA, 1        ; Set RA1 as output (TRISA = xxxx xx00)
    BCF TRISA, 2        ; Set RA2 as output (TRISA = xxxx x000)
    RETURN

; Button check
btn_branch_not_pressed macro label
    BTFSC PORTB, 0      ; Check if PORTB bit 0 is low (button pressed)
    GOTO label          ; If button is not pressed, branch label
    endm

btn_branch_pressed macro label
    BTFSS PORTB, 0      ; Check if PORTB bit 0 is high (button not pressed)
    GOTO label          ; If button is pressed, branch label
    endm

basic:
    RCALL init
ba_state0:
    btn_branch_not_pressed ba_state0        ; branch back if button is not pressed
    BSF LATA, 0                             ; light up RA0
    DELAY d'111', d'56'                     ; delay 0.1 sec for handling bouncing
ba_state1:
    btn_branch_not_pressed ba_state1        ; branch back if button is not pressed
    BCF LATA, 0                             ; extinguish RA0
    BSF LATA, 1                             ; light up RA1
    DELAY d'111', d'56'                     ; delay 0.1 sec for handling bouncing
ba_state2:
    btn_branch_not_pressed ba_state2        ; branch back if button is not pressed
    BCF LATA, 1                             ; extinguish RA1
    BSF LATA, 2                             ; light up RA2
    DELAY d'111', d'56'                     ; delay 0.2 sec for handling bouncing
ba_state3:
    btn_branch_not_pressed ba_state3        ; branch back if button is not pressed
    BCF LATA, 2                             ; extinguish RA2
    DELAY d'111', d'56'                     ; delay 0.2 sec for handling bouncing
    GOTO ba_state0

; --------------------------- ADVENCED ---------------------------

advanced:
    RCALL init
ad_state0:
    btn_branch_not_pressed ad_state0            ; branch back if button is not pressed
    BSF LATA, 0                                 ; light up RA0
ad_state1:
    DELAY d'111', d'28'                         ; delay 0.1 sec for handling bouncing
    DELAY_WITH_CHECK d'111', d'182', ad_back0   ; delay 0.65 sec
    BCF LATA, 0                                 ; extinguish RA0
    BSF LATA, 1                                 ; light up RA1
ad_state2:
    DELAY d'111', d'28'                         ; delay 0.1 sec for handling bouncing
    DELAY_WITH_CHECK d'111', d'182', ad_back0   ; delay 0.65 sec
    BCF LATA, 1                                 ; extinguish RA1
    BSF LATA, 2                                 ; light up RA2
ad_state3:
    DELAY d'111', d'28'                         ; delay 0.1 sec for handling bouncing
    DELAY_WITH_CHECK d'111', d'182', ad_back0   ; delay 0.65 sec
ad_back0:
    DELAY d'111', d'28'                         ; delay 0.1 sec for handling bouncing
    BCF LATA, 0
    BCF LATA, 1
    BCF LATA, 2
    GOTO ad_state0

; --------------------------- BONUS ---------------------------

bonus:
    RCALL init
bo_state0:
    btn_branch_not_pressed bo_state0            ; branch back if button is not pressed
    BSF LATA, 0                                 ; light up RA0
bo_state1_1:
    DELAY d'111', d'28'                         ; delay 0.1 sec for handling bouncing
    DELAY_WITH_CHECK d'111', d'112', bo_state2  ; delay 0.4 sec
    BCF LATA, 0                                 ; extinguish RA0
    BSF LATA, 1                                 ; light up RA1
bo_state1_2:
    DELAY d'111', d'28'                         ; delay 0.1 sec for handling bouncing
    DELAY_WITH_CHECK d'111', d'112', bo_state2  ; delay 0.4 sec
    BCF LATA, 1                                 ; extinguish RA1
    BSF LATA, 2                                 ; light up RA2
bo_state1_3:
    DELAY d'111', d'28'                         ; delay 0.1 sec for handling bouncing
    DELAY_WITH_CHECK d'111', d'112', bo_state2  ; delay 0.4 sec
    BCF LATA, 2                                 ; extinguish RA2
    BSF LATA, 0                                 ; light up RA0
    GOTO bo_state1_1
bo_state2:
    BSF LATA, 0                                 ; light up RA0
    BCF LATA, 1                                 ; extinguish RA1 and 2
    BCF LATA, 2
bo_state2_1:
    DELAY d'111', d'28'                         ; delay 0.1 sec for handling bouncing
    DELAY_WITH_CHECK d'111', d'252', bo_back0   ; delay 0.9 sec
    BCF LATA, 0                                 ; extinguish RA0
    BSF LATA, 1                                 ; light up RA1
bo_state2_2:
    DELAY d'111', d'28'                         ; delay 0.1 sec for handling bouncing
    DELAY_WITH_CHECK d'111', d'252', bo_back0   ; delay 0.9 sec
    BCF LATA, 1                                 ; extinguish RA1
    BSF LATA, 2                                 ; light up RA2
bo_state2_3:
    DELAY d'111', d'28'                         ; delay 0.1 sec for handling bouncing
    DELAY_WITH_CHECK d'111', d'112', bo_back0   ; delay 0.4 sec
    BCF LATA, 2                                 ; extinguish RA2
    DELAY_WITH_CHECK d'111', d'140', bo_back0   ; delay 0.5 sec
    BSF LATA, 2                                 ; light up RA2
    DELAY_WITH_CHECK d'111', d'140', bo_back0   ; delay 0.5 sec
    BCF LATA, 2                                 ; extinguish RA2
    DELAY_WITH_CHECK d'111', d'140', bo_back0   ; delay 0.5 sec
    BSF LATA, 2                                 ; light up RA2
    DELAY_WITH_CHECK d'111', d'140', bo_back0   ; delay 0.5 sec
    GOTO bo_state2
bo_back0:
    DELAY d'111', d'28'             ; delay 0.1 sec for handling bouncing
    BCF LATA, 0
    BCF LATA, 1
    BCF LATA, 2
    GOTO bo_state0

    end

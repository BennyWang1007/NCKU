#include "p18f4520.inc"

; CONFIG
  CONFIG  OSC = INTIO67         ; Oscillator Selection bits (Internal oscillator block, port function on RA6 and RA7)
  CONFIG  PWRT = OFF            ; Power-up Timer Enable bit (PWRT disabled)
  CONFIG  WDT = OFF             ; Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
  CONFIG  BOREN = SBORDIS       ; Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
  CONFIG  PBADEN = ON           ; PORTB A/D Enable bit (PORTB<4:0> pins are configured as analog input channels on Reset)
  CONFIG  LVP = OFF             ; Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
  CONFIG  CPD = OFF             ; Data EEPROM Code Protection bit (Data EEPROM not code-protected)

; ----------------------------- VARIABLES -----------------------------
L1	EQU 0x14         ; Define L1 memory location
L2	EQU 0x15         ; Define L2 memory location

    ORG 0x00            ; Set program start address to 0x00
main:
;	#define BASIC
;	#define ADVANCED
;	#define BONUS

	RCALL init
#ifdef BASIC
	RCALL basic
#endif
#ifdef ADVANCED
	RCALL advanced
#endif
#ifdef BONUS
	RCALL bonus
#endif


; delay = (2+(5+8*num1)*num2) * 4 mu s
DELAY macro num1, num2
    local LOOP1, LOOP2
    MOVLW num2
    MOVWF L2
LOOP2
    MOVLW num1
    MOVWF L1
LOOP1
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

MOVLF macro literal, F
	MOVLW literal
	MOVWF F
	endm

SWAPFF macro f1, f2, tmp
	MOVFF f1, tmp
	MOVFF f2, f1
	MOVFF tmp, f2
	endm

SWAPFF3 macro f1, f2, f3, tmp
	MOVFF f1, tmp
	MOVFF f2, f1
	MOVFF f3, f2
	MOVFF tmp, f3
	endm


init:
	BSF T2CON, TMR2ON
	BCF T2CON, T2CKPS1	; T2CON.T2CKPS = 10
	BSF T2CON, T2CKPS0
	
	BCF OSCCON, IRCF2	; OSCCON.IRCF = 001
	BCF OSCCON, IRCF1
	BSF OSCCON, IRCF0
	
	BSF CCP1CON, CCP1M3	; CCP1CON.CCP1M = 1100
	BSF CCP1CON, CCP1M2
	BCF CCP1CON, CCP1M1
	BCF CCP1CON, CCP1M0
	
	CLRF TRISC			; make port C output
	CLRF LATC
	
	; PWM period
    ; = (PR2 + 1) * 4 * Tosc * (TMR2 prescaler)
    ; = (0x9b + 1) * 4 * 8µs * 4
    ; = 0.019968s ~= 20ms
	MOVLF 0x9b, PR2
	
	; Duty cycle
    ; = (CCPR1L:CCP1CON<5:4>) * Tosc * (TMR2 prescaler)
    ; = (0x0b*4 + 0b01) * 8µs * 4
    ; = 0.00144s ~= 1450µs
	MOVLF 0x0b, CCPR1L
	BCF CCP1CON, DC1B1		; CCP1CON.DC1B = 01
	BSF CCP1CON, DC1B0
	
	RETURN
	
basic:
	
	MOVLF 0x0, CCPR1L
ba_loop:
	DELAY d'112', d'112'
	INCF CCPR1L
	BTFSC CCPR1L, 4
	CLRF CCPR1L
	BRA ba_loop
	

	
	end


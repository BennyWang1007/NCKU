#include "p18f4520.inc"

; CONFIG1H
  CONFIG  OSC = INTIO67         ; Oscillator Selection bits (Internal oscillator block, port function on RA6 and RA7)
  CONFIG  FCMEN = OFF           ; Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
  CONFIG  IESO = OFF            ; Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

; CONFIG2L
  CONFIG  PWRT = OFF            ; Power-up Timer Enable bit (PWRT disabled)
  CONFIG  BOREN = SBORDIS       ; Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
  CONFIG  BORV = 3              ; Brown Out Reset Voltage bits (Minimum setting)

; CONFIG2H
  CONFIG  WDT = OFF             ; Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
  CONFIG  WDTPS = 32768         ; Watchdog Timer Postscale Select bits (1:32768)

; CONFIG3H
  CONFIG  CCP2MX = PORTC        ; CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
  CONFIG  PBADEN = ON           ; PORTB A/D Enable bit (PORTB<4:0> pins are configured as analog input channels on Reset)
  CONFIG  LPT1OSC = OFF         ; Low-Power Timer1 Oscillator Enable bit (Timer1 configured for higher power operation)
  CONFIG  MCLRE = ON            ; MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)

; CONFIG4L
  CONFIG  STVREN = ON           ; Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
  CONFIG  LVP = OFF             ; Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
  CONFIG  XINST = OFF           ; Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

; CONFIG5L
  CONFIG  CP0 = OFF             ; Code Protection bit (Block 0 (000800-001FFFh) not code-protected)
  CONFIG  CP1 = OFF             ; Code Protection bit (Block 1 (002000-003FFFh) not code-protected)
  CONFIG  CP2 = OFF             ; Code Protection bit (Block 2 (004000-005FFFh) not code-protected)
  CONFIG  CP3 = OFF             ; Code Protection bit (Block 3 (006000-007FFFh) not code-protected)

; CONFIG5H
  CONFIG  CPB = OFF             ; Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
  CONFIG  CPD = OFF             ; Data EEPROM Code Protection bit (Data EEPROM not code-protected)

; CONFIG6L
  CONFIG  WRT0 = OFF            ; Write Protection bit (Block 0 (000800-001FFFh) not write-protected)
  CONFIG  WRT1 = OFF            ; Write Protection bit (Block 1 (002000-003FFFh) not write-protected)
  CONFIG  WRT2 = OFF            ; Write Protection bit (Block 2 (004000-005FFFh) not write-protected)
  CONFIG  WRT3 = OFF            ; Write Protection bit (Block 3 (006000-007FFFh) not write-protected)

; CONFIG6H
  CONFIG  WRTC = OFF            ; Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
  CONFIG  WRTB = OFF            ; Boot Block Write Protection bit (Boot block (000000-0007FFh) not write-protected)
  CONFIG  WRTD = OFF            ; Data EEPROM Write Protection bit (Data EEPROM not write-protected)

; CONFIG7L
  CONFIG  EBTR0 = OFF           ; Table Read Protection bit (Block 0 (000800-001FFFh) not protected from table reads executed in other blocks)
  CONFIG  EBTR1 = OFF           ; Table Read Protection bit (Block 1 (002000-003FFFh) not protected from table reads executed in other blocks)
  CONFIG  EBTR2 = OFF           ; Table Read Protection bit (Block 2 (004000-005FFFh) not protected from table reads executed in other blocks)
  CONFIG  EBTR3 = OFF           ; Table Read Protection bit (Block 3 (006000-007FFFh) not protected from table reads executed in other blocks)

; CONFIG7H
  CONFIG  EBTRB = OFF           ; Boot Block Table Read Protection bit (Boot block (000000-0007FFh) not protected from table reads executed in other blocks) 

; ----------------------------- VARIABLES -----------------------------
L1  EQU 0x14         ; Define L1 memory location
L2  EQU 0x15         ; Define L2 memory location
LIGHT_TARGET    EQU 0x30
BONUS_SWAP_TMP  EQU 0x31
AD_TIMEOUT_NEXT EQU 0x32
AD_TIMEOUT_NEXT2 EQU 0x33
BO_TIMEOUT_NEXT EQU 0x32
BO_START_CUR    EQU 0x33
BO_START_NEXT   EQU 0x34
BO_START_NEXT2  EQU 0x35
BO_END_CUR      EQU 0x36
BO_END_NEXT     EQU 0x37
BO_END_NEXT2    EQU 0x38
BO_INC_CUR      EQU 0x39
BO_INC_NEXT     EQU 0x3A
BO_INC_NEXT2    EQU 0x3B
BO_MASK_CUR     EQU 0x3C
BO_MASK_NEXT    EQU 0x3D
BO_MASK_NEXT2   EQU 0x3E
BASIC_HALT EQU 0x40

    ORG 0x00            ; Set program start address to 0x00
main:
;   #define BASIC
;   #define ADVANCED
    #define BONUS

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

ISR:
    ORG 0x08
#ifdef BASIC
    BRA BASIC_ISR
#endif
#ifdef ADVANCED
    BRA AD_ISR
#endif
#ifdef BONUS
    BRA BONUS_ISR
#endif
    RETFIE

BASIC_ISR:
    CLRF LATA
    CLRF BASIC_HALT
    RLNCF LIGHT_TARGET      ; 4 -> 8 -> 16 -> 32(->4)
    MOVLW b'011100'
    BTFSC LIGHT_TARGET, 5   ; if 32 -> 4
    SUBWF LIGHT_TARGET, F   ; LIGHT_TARGET = 0b100000-0b011100 = 0b100
    BCF INTCON, INT0IF      ; clear int flag
    RETFIE

AD_ISR:
    INCF LATA
    MOVF LATA, W
    CPFSEQ LIGHT_TARGET
    BRA AD_ISR_END
ad_next_state:
    SWAPFF3 PR2, AD_TIMEOUT_NEXT, AD_TIMEOUT_NEXT2, BONUS_SWAP_TMP
    RLCF LIGHT_TARGET
    MOVLW b'11100'
    BTFSS LIGHT_TARGET, 5
    BRA AD_ISR_END              ; branch to end if bit 5 is clear
    SUBWF LIGHT_TARGET, F
    CLRF LATA
AD_ISR_END:
    BCF PIR1, TMR2IF            ; clear timer2 flag
    RETFIE

BONUS_ISR:
    BTFSC PIR1, TMR2IF          ; skip if not timeout
    RCALL BONUS_TIMEOUT
    BTFSC INTCON, INT0IF        ; skip if buttom not pressed
    RCALL BONUS_BUTTOM
    BCF INTCON, INT0IF          ; clear int flag
    BCF PIR1,    TMR2IF         ; clear timer2 flag
    RETFIE

BONUS_TIMEOUT:
    MOVF BO_INC_CUR, W
    ADDWF LATA, F
    MOVF BO_MASK_CUR, W
    ANDWF LATA, F
    RETURN

BONUS_BUTTOM:
    SWAPFF  PR2,          BO_TIMEOUT_NEXT, BONUS_SWAP_TMP
    SWAPFF3 BO_START_CUR, BO_START_NEXT,  BO_START_NEXT2, BONUS_SWAP_TMP
    SWAPFF3 BO_END_CUR,   BO_END_NEXT,    BO_END_NEXT2,   BONUS_SWAP_TMP
    SWAPFF3 BO_INC_CUR,   BO_INC_NEXT,    BO_INC_NEXT2,   BONUS_SWAP_TMP
    SWAPFF3 BO_MASK_CUR,  BO_MASK_NEXT,   BO_MASK_NEXT2,  BONUS_SWAP_TMP
    MOVF BO_START_CUR, W
    MOVWF LATA
    RETURN

init:
    ; I/O setting
    MOVLW 0x0f
    MOVWF ADCON1        ; Set 1's to ADCON1 register -> Digital
    BSF TRISB, 0        ; Set RB0 as input  (TRISB = xxxx xxx1)
    CLRF LATA           ; Clear LATA
    MOVLW 0xf0
    ANDWF TRISA, F      ; Set RA0-3 as output (TRISA = xxxx 0000)
    ; interrupt setting
    BSF RCON, IPEN
    BCF INTCON, INT0IF      ; Clear Int0 Int Flag
    BSF INTCON, GIE         ; Set Global Interupt Enable
    BSF INTCON, INT0IE      ; Set Int0 Interupt Enable
    
    RETURN

timer2_init:
    ; Timer setting
    BCF PIR1, TMR2IF    ; Clear flag of timer2
    BSF IPR1, TMR2IP    ; Set prioritized timer2
    BSF PIE1, TMR2IE    ; Set enable bit of timer2
    ; Timer2 setting
    MOVLW b'0111111'    ; 7: not implement; 6-3: postscaler, 2: enable, 1-0: prescaler
    MOVWF T2CON
;   MOVLW d'122'        ; 0.5 s * 250 kHz = 125000/4 = 31250 cycle. 31250 / 16 / 16 = 122 
;   MOVWF PR2
    ; OSC setting
    MOVLF b'00100000', OSCCON
    RETURN

basic:
    CLRF LATA
    BSF BASIC_HALT, 0
    MOVLF d'4', LIGHT_TARGET
basic_loop:
    BTFSC BASIC_HALT, 0
    BRA basic_loop
    MOVF LATA, W
    INCF WREG
    CPFSEQ LIGHT_TARGET
    BRA ba_end
ba_halt:
    INCF BASIC_HALT
    CLRF LATA
    BRA basic_loop
ba_end:
    MOVWF LATA
    DELAY d'111', d'28' ; 0.2s
    BRA basic_loop

advanced:
    RCALL timer2_init
    CLRF LATA
    MOVLF d'4', LIGHT_TARGET    ; target = 3
    MOVLF d'61', PR2            ; timeout = 0.25s
    MOVLF d'122', AD_TIMEOUT_NEXT
    MOVLF d'244', AD_TIMEOUT_NEXT2
advanced_loop:
    BRA advanced_loop

BONUS_INIT macro start_l, end_l, inc_l, mask_l, start_f, end_f, inc_f, mask_f
    MOVLF start_l,  start_f
    MOVLF end_l,    end_f
    MOVLF inc_l,    inc_f
    MOVLF mask_l,   mask_f
    endm

bonus:
    RCALL timer2_init
    MOVLF d'61', PR2
    MOVLF d'122', BO_TIMEOUT_NEXT
    BONUS_INIT 0x0, 0x7, 0x01, 0x7, BO_START_CUR,   BO_END_CUR,   BO_INC_CUR,   BO_MASK_CUR     ; 0-7 with increment 1
    BONUS_INIT 0x0, 0xf, 0x01, 0xf, BO_START_NEXT,  BO_END_NEXT,  BO_INC_NEXT,  BO_MASK_NEXT    ; 0-f with increment 1
    BONUS_INIT 0xf, 0x0, 0xff, 0xf, BO_START_NEXT2, BO_END_NEXT2, BO_INC_NEXT2, BO_MASK_NEXT2   ; f-0 with increment -1
bonus_loop:
    BRA bonus_loop

    end

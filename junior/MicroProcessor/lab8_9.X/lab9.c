#include <xc.h>
#include <pic18f4520.h>
#include <stdio.h>

#pragma config OSC = INTIO67 // Oscillator Selection bits
#pragma config WDT = OFF     // Watchdog Timer Enable bit
#pragma config PWRT = OFF    // Power-up Enable bit
#pragma config BOREN = ON    // Brown-out Reset Enable bit
#pragma config PBADEN = OFF  // Watchdog Timer Enable bit
#pragma config LVP = OFF     // Low Voltage (single -supply) In-Circute Serial Pragramming Enable bit
#pragma config CPD = OFF     // Data EEPROM?Memory Code Protection bit (Data EEPROM code protection off)

typedef unsigned char uint8;
typedef char int8;
typedef unsigned int uint16;
typedef int int16;

// #define BASIC
// #define ADVANCED
#define BONUS

#define RB0_PRESSED 0

int8 difficulty = 0;

void basic_ISR() {
    int8 quotient = ADRESH / 32;
    static const int8 lata_map[] = {2, 5, 1, 6, 1, 1, 4, 7};
    LATA = lata_map[quotient];
    LATA *= 2;
    return;
}

void advanced_ISR() {
    int8 voltage = ADRESH;
    static int8 prev_voltage;
    if (voltage == prev_voltage) return;
    if (voltage < prev_voltage) {
        LATA = (voltage / 32) * 2;      // 0, 2, ..., 14
    } else {
        LATA = (voltage / 32) * 2 + 1;  // 1, 3, ..., 15
    }
    LATA *= 2;
    prev_voltage = voltage;
}

void bonus_ISR() {
    // map 0-128, 255-129 to 0-255
    if (ADRESH == 0) {
        CCPR1L = 0;
        return;
    }
    int value = (ADRESH - 128);
    if (value < 0) value = -value;
    value = 255 - value * 2;
    CCPR1L = value;
}


void __interrupt(high_priority)H_ISR(){

    //step4
#ifdef BASIC
    basic_ISR();
#endif     
#ifdef ADVANCED
    advanced_ISR();
#endif
#ifdef BONUS
    bonus_ISR();
#endif

    // clear flag bit
    PIR1bits.ADIF = 0;

    // step5 & go back step3
    // delay at least 2tad
    for (int16 i = 0; i < 2000; ++i);
    ADCON0bits.GO = 1;

    return;
}


void main(void) {
    // configure OSC and port
    OSCCONbits.IRCF = 0b100; // 1MHz
    TRISAbits.RA0 = 1;       // analog input port

    // step1
    ADCON1bits.VCFG0 = 0;
    ADCON1bits.VCFG1 = 0;
    ADCON1bits.PCFG = 0b1110; // set AN0 as analog input, others as digital
    ADCON0bits.CHS = 0b0000;  // set AN0 as analog input
    ADCON2bits.ADCS = 0b000;  // set to 000 (1Mhz < 2.86Mhz) by the sheet
    ADCON2bits.ACQT = 0b001;  // Tad = 2 us acquisition time?2Tad = 4 > 2.4
    ADCON0bits.ADON = 1;
    ADCON2bits.ADFM = 0;    // left justified 

    // step2
    PIE1bits.ADIE = 1;
    PIR1bits.ADIF = 0;
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;

    // step3
    ADCON0bits.GO = 1;

    // init LED, set RA1-5 as output
    TRISA = 0b00000001;
    LATA = 0;

#ifdef BONUS
    T2CONbits.TMR2ON = 0b1;
    T2CONbits.T2CKPS = 0b00;

    // Internal Oscillator Frequency, Fosc = 125 kHz, Tosc = 8 microseconds
    OSCCONbits.IRCF = 0b001;

    // PWM mode, P1A, P1C active-high; P1B, P1D active-high
    CCP1CONbits.CCP1M = 0b1100;

    CCP1CONbits.DC1B = 0b00;

    // CCP1/RC2 -> Output
    TRISC = 0;
    LATC = 0;
#endif

    while(1);

    return;
}
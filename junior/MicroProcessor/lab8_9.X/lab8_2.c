#include <xc.h>
#include <pic18f4520.h>

#pragma config OSC = INTIO67    // Oscillator Selection bits
#pragma config WDT = OFF        // Watchdog Timer Enable bit 
#pragma config PWRT = OFF       // Power-up Enable bit
#pragma config BOREN = ON       // Brown-out Reset Enable bit
#pragma config PBADEN = OFF     // Watchdog Timer Enable bit 
#pragma config LVP = OFF        // Low Voltage (single -supply) In-Circute Serial Pragramming Enable bit
#pragma config CPD = OFF        // Data EEPROM?Memory Code Protection bit (Data EEPROM code protection off)

typedef unsigned char uint8;
typedef char int8;
typedef unsigned int uint16;
typedef int int16;

int8 BASIC_STATE = 0;

void __interrupt() ISR(void) {
    switch (BASIC_STATE) {
        case 0:
            BASIC_STATE++;
            set_degree(0);
            break;
        case 1:
            BASIC_STATE++;
            set_degree(90);
            break;
        case 2:
            BASIC_STATE++;
            set_degree(0);
            break;
        case 3:
            BASIC_STATE = 0;
            set_degree(-90);
            break;
        default:
            break;
    }
    
}

void set_degree(int8 degree) {
    uint16 value = 500 + (degree+90) * 1900;
    CCPR1L = (uint8)(value >> 2);
    CCP1CONbits.DC1B = value % 4;
}


//void main(void)
//{
//    // Timer2 -> On, prescaler -> 4
//    T2CONbits.TMR2ON = 0b1;
//    T2CONbits.T2CKPS = 0b01;
//
//    // Internal Oscillator Frequency, Fosc = 125 kHz, Tosc = 8 탎
//    OSCCONbits.IRCF = 0b001;
//    
//    // PWM mode, P1A, P1C active-high; P1B, P1D active-high
//    CCP1CONbits.CCP1M = 0b1100;
//    
//    // CCP1/RC2 -> Output
//    TRISC = 0;
//    LATC = 0;
//    
//    // Set up PR2, CCP to decide PWM period and Duty Cycle
//    /** 
//     * PWM period
//     * = (PR2 + 1) * 4 * Tosc * (TMR2 prescaler)
//     * = (0x9b + 1) * 4 * 8탎 * 4
//     * = 0.019968s ~= 20ms
//     */
//    PR2 = 0x9b;
//    
//    /**
//     * Duty cycle
//     * = (CCPR1L:CCP1CON<5:4>) * Tosc * (TMR2 prescaler)
//     * = (0x0b*4 + 0b01) * 8탎 * 4
//     * = 0.00144s ~= 1450탎
//     */
//    CCPR1L = 0x08;
//    CCP1CONbits.DC1B = 0b01;
//    
//    while(1);
//    return;
//}
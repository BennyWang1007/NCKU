#include <xc.h>
#include <pic18f4520.h>

#pragma config OSC = INTIO67 // Oscillator Selection bits
#pragma config WDT = OFF     // Watchdog Timer Enable bit
#pragma config PWRT = OFF    // Power-up Enable bit
#pragma config BOREN = ON    // Brown-out Reset Enable bit
#pragma config PBADEN = OFF  // Watchdog Timer Enable bit
#pragma config LVP = OFF     // Low Voltage (single -supply) In-Circute Serial Pragramming Enable bit
#pragma config CPD = OFF     // Data EEPROM?Memory Code Protection bit (Data EEPROM code protection off)

//#define BASIC 1
#define ADVANCED 1
//#define BONUS 1

#define RB0_PRESSED 0
#define ROTATE_NOP_COUNT 5

typedef unsigned char uint8;
typedef char int8;
typedef unsigned int uint16;
typedef int int16;

/**
 * Duty cycle
 * = (CCPR1L:CCP1CON<5:4>) * Tosc * (TMR2 prescaler)
 * = (0x0b*4 + 0b01) * 8 mu_s * 4
 * = 0.00144s ~= 1450 mu_s
 */
// 500  -> -90  => CCPR1L = 0X04
// 1450 ->   0  => CCPR1L = 0X0B
// 2400 -> +90  => CCPR1L = 0X14
static inline void set_degree(int16 degree) {
    uint16 value = ((uint16)190 * (uint16)(degree + 90) / 18 + 500) / 32;
    CCPR1L = (uint8)(value / 4);
    CCP1CONbits.DC1B = value % 4;
}


void main(void) {
    // Timer2 -> On, prescaler -> 4
    T2CONbits.TMR2ON = 0b1;
    T2CONbits.T2CKPS = 0b01;

    // Internal Oscillator Frequency, Fosc = 125 kHz, Tosc = 8 microseconds
    OSCCONbits.IRCF = 0b001;

    // PWM mode, P1A, P1C active-high; P1B, P1D active-high
    CCP1CONbits.CCP1M = 0b1100;

    // CCP1/RC2 -> Output
    TRISC = 0;
    LATC = 0;
    TRISB = 0X01;//RB0 = input

    // Set up PR2, CCP to decide PWM period and Duty Cycle
    /**
     * PWM period
     * = (PR2 + 1) * 4 * Tosc * (TMR2 prescaler)
     * = (0x9b + 1) * 4 * 8 mu_s * 4
     * = 0.019968s ~= 20ms
     */
    PR2 = 0x9b;

#ifdef BASIC
    CCPR1L = 0x04;
    CCP1CONbits.DC1B = 0b01;
    int basic_state = 0;
    while (1) {
        if (PORTBbits.RB0 == RB0_PRESSED) {
            switch (basic_state++) {
            case 0:                 // -90 -> 0
                CCPR1L = 0x0b;
                break;
            case 1:                 // 0 -> 90
                CCPR1L = 0x14;
                break;
            case 2:                 // 90 -> 0
                CCPR1L = 0x0b;
                break;
            case 3:                 // 0 -> -90
                basic_state = 0;
                CCPR1L = 0x04;
                break;
            }
            _delay(10000);
        }
    }
#endif

#ifdef ADVANCED
    CCPR1L = 0x04;
    CCP1CONbits.DC1B = 0b01;
    while (1) {
        if (PORTBbits.RB0 == RB0_PRESSED) {
            for (int degree = -90; degree <= 90; ++degree) {
                set_degree(degree);
                _delay(ROTATE_NOP_COUNT);
            }
            set_degree(-90);
        }
    }
#endif

#ifdef BONUS
    CCPR1L = 0x04;
    CCP1CONbits.DC1B = 0b01;
    while (1) {
        if (PORTBbits.RB0 == RB0_PRESSED) {
            while (1) {
                for (int degree = -90; degree <= 90; ++degree) {
                    set_degree(degree);
                    _delay(ROTATE_NOP_COUNT);
                }
                for (int degree = 89; degree > -90; --degree) {
                    set_degree(degree);
                    _delay(ROTATE_NOP_COUNT);
                }
            }
        }
    }
#endif
    return;
}


//#include <xc.h>
//#include <pic18f4520.h>
#include "setting_hardaware/setting.h"
#include <stdlib.h>
#include "stdio.h"
#include "string.h"

#pragma config OSC = INTIO67 // Oscillator Selection bits
#pragma config WDT = OFF     // Watchdog Timer Enable bit
#pragma config PWRT = OFF    // Power-up Enable bit
#pragma config BOREN = ON    // Brown-out Reset Enable bit
#pragma config PBADEN = OFF  // Watchdog Timer Enable bit
#pragma config LVP = OFF     // Low Voltage (single -supply) In-Circute Serial Pragramming Enable bit
#pragma config CPD = OFF     // Data EEPROM?Memory Code Protection bit (Data EEPROM code protection off)


// #define RB0_PRESSED (PORTBbits.RB0 == 0)
// #define _XTAL_FREQ 125000

// typedef unsigned char uint8;
// typedef char int8;
// typedef unsigned int uint16;
// typedef int int16;

//#include "final_111.c"
//#include "final_112.c"
#include "final_113.c"

#include "utils.c"


void __interrupt(high_priority) Hi_ISR(void) {
    // final_112_1A_int();
    // final_112_2A_int();
    // final_111_1A_int();
    // final_111_1B_int();
    // final_111_2A_int();
    // final_113_1A_int();
    // final_113_1B_int();
    // final_113_2A_int();
     final_113_2B_int();
//    I2C_int();
    return;
}

void step_motor_test();

void main(void) {

    SYSTEM_Initialize();
//
//    // Timer2 -> On, prescaler -> 4
    T2CONbits.TMR2ON = 0b1;
    T2CONbits.T2CKPS = 0b01;
//
//    // PWM mode, P1A, P1C active-high; P1B, P1D active-high
    CCP1CONbits.CCP1M = 0b1100;
//
//    // enable int0
    INTCONbits.INT0IF = 0;
    INTCONbits.INT0IE = 1;
    INTCONbits.GIE = 1;
//
//    
    TRISC &= 0b11111011;    // CCP1/RC2 -> output
    TRISB |= 0b00000001;    // RB0 = input
    LATA = 0;
    LATC = 0;

    // Set up PR2, CCP to decide PWM period and Duty Cycle
    /**
     * PWM period
     * = (PR2 + 1) * 4 * Tosc * (TMR2 prescaler)
     * = (0x9b + 1) * 4 * 8 mu_s * 4
     * = 0.019968s ~= 20ms
     */
    PR2 = 0x9b;

    /**
     * Duty cycle
     * = (CCPR1L:CCP1CON<5:4>) * Tosc * (TMR2 prescaler)
     * = (0x0b*4 + 0b01) * 8 mu_s * 4
     * = 0.00144s ~= 1450 mu_s
     */
    // 500  -> -90  => CCPR1L = 0X04
    // 1450 ->   0  => CCPR1L = 0X0B
    // 2400 -> +90  => CCPR1L = 0X14

    // input_test();
    // send_number_test();

    // final_111_1A();
    // final_111_1B();
    // final_111_2A();
    // final_111_2B();
    // final_111_3A();
    // final_111_3B();

    // final_112_1A();
    // final_112_2A();
    // final_112_2B();
    // final_112_3A();
    // final_112_3B();

    // final_113_1A();
    // final_113_1B();
    // final_113_2A();
     final_113_2B();
    // final_113_3A();
//     final_113_3B();

    TRISC = 0x00;
    TRISD = 0x00;
    LATD = 0x00;

    LATDbits.LATD3 = 1;
    
//    step_motor_test();
    
    // while (1) {
    //     LATDbits.LATD3 = ~LATDbits.LATD3;
    //     __delay_ms(1000);
    // }

//    I2C_read_test();
    
    while(1);

    return;
}


#define MOTOR_STEP 2048
#define MOTOR_DELAY 100

int16 accumulated_degree = 0;
int16 accumulated_step = 0;
int16 target_step = 0;

void step_motor_step(int16 degree) {
    accumulated_degree += degree;
    target_step = degree * MOTOR_STEP / 360;
    int16 step = target_step - accumulated_step;
    if (step < 0) step += MOTOR_STEP;
    for (int i = 0; i < step; i++) {
        if (LATC == 0x08) LATC = 0x01;
        else LATC <<= 1;
        __delay_ms(MOTOR_DELAY);
    }
    accumulated_step = target_step % MOTOR_STEP;
}

int8 phase = 0;

void step_motor_test() {
    int16 degree;
    int16 counter = 0;
    while (1) {
        // degree = get_int16_input();
        // degree = 100;
        // step_motor_step(degree);
        switch (phase) {
        case 0:
            // LATC = 0b00001110;
            LATC = 0x01;
            phase += 1;
            break;
        case 1:
            // LATC = 0b00001101;
            LATC = 0x02;
            phase += 1;
            break;
        case 2:
            // LATC = 0b00001011;
            LATC = 0x04;
            phase += 1;
            break;
        case 3:
            // LATC = 0b00000111;
            LATC = 0x08;
            phase = 0;
            break;
        default:
            phase = 0;
            break;
        }
        counter ++;
        if (counter == MOTOR_STEP) break;
        __delay_ms(MOTOR_DELAY);
        
        // LATC = 0x01;
        // LATD = 1;
        // __delay_ms(MOTOR_DELAY);
        // LATC = 0x02;
        // LATD = 2;
        // __delay_ms(MOTOR_DELAY);
        // LATC = 0x04;
        // LATD = 3;
        // __delay_ms(MOTOR_DELAY);
        // LATC = 0x08;
        // LATD = 4;
        // __delay_ms(MOTOR_DELAY);
    }

}


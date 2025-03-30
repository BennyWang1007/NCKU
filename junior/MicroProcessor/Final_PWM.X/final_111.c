#include "setting_hardaware/setting.h"
#include <stdlib.h>
#include "stdio.h"
#include "string.h"

#include "utils.c"


// #define RB0_PRESSED (PORTBbits.RB0 == 0)
// #define _XTAL_FREQ 125000

// typedef unsigned char uint8;
// typedef char int8;
// typedef unsigned int uint16;
// typedef int int16;


void final_111_1A() {
    int8 x = 13, t = 1;
    set_timer2_ms(1000 * t);
    init_timer2();
    set_LED_display(x);
    while (1);
}

void final_111_1A_int() {
    if (PIR1bits.TMR2IF) {
        PIR1bits.TMR2IF = 0;
        timer2_postscaler_counter++;
        if (timer2_postscaler_counter < TIMER2_POSTSCALER) return;
        timer2_postscaler_counter = 0;
        uint8 num = get_LED_display();
        if (num == 0) return;
        set_LED_display(num - 1);
    }
}

void final_111_1B() {


    int8 x = 13, t = 1;

    set_timer2_ms(1000 * t);

    // set_timer2_sec(t);
    init_timer2();

    set_LED_display(x);

    while (1) {
        int8 input = get_int8_input();
        switch (input) {
            case 0:
                set_timer2_ms(250);
                break;
            case 1:
                set_timer2_ms(500);
                break;
            case 2:
                set_timer2_ms(1000);
                break;
            default:
                break;
        }
    }
}

void final_111_1B_int() {
    if (PIR1bits.TMR2IF) {
        PIR1bits.TMR2IF = 0;
        timer2_postscaler_counter++;
        if (timer2_postscaler_counter < TIMER2_POSTSCALER) return;
        timer2_postscaler_counter = 0;

        uint8 num = get_LED_display();
        if (num == 1) set_LED_display(2);
        else set_LED_display(1);
    }
}


int8 final_111_2A_n = 9;
int8 final_111_2A_cur_cycle = 1;
int8 final_111_2A_phase = 0;


void final_111_2A() {
    set_timer2_ms(1000);
    init_timer2();
    while (1);
}

void final_111_2B() {
    final_111_2A_n = get_int8_input();
    final_111_2A_cur_cycle = 1;
    final_111_2A_phase = 0;
    final_111_2A();
}

void final_111_2A_int() {
    if (PIR1bits.TMR2IF) {
        PIR1bits.TMR2IF = 0;
        timer2_postscaler_counter++;
        if (timer2_postscaler_counter < TIMER2_POSTSCALER) return;
        timer2_postscaler_counter = 0;
        
        set_LED_display(final_111_2A_cur_cycle);

        int16 angle;
        switch (final_111_2A_phase) {
        case 0:
            angle = (int16)final_111_2A_cur_cycle * -90 / final_111_2A_n;
            break;
        case 1:
        case 3:
            angle = 0;
            break;
        case 2:
            angle = (int16)final_111_2A_cur_cycle * 90 / final_111_2A_n;
            break;
        default:
            return;
        }
        set_degree(angle);
        final_111_2A_phase++;
    }
    else if (INTCONbits.INT0IF) {
        INTCONbits.INT0IF = 0;
        if (final_111_2A_cur_cycle == final_111_2A_n) return;
        final_111_2A_cur_cycle++;
        final_111_2A_phase = 0;
    }

}


void final_111_3A() {
    int16 voltage, degree;
    while (1) {
        voltage = ADC_Read(0);
        degree = ((unsigned long)voltage - 512) * 90 / 512;
        set_degree(degree);
    }
}

void final_111_3B() {
    int16 voltage, degree;
    while (1) {
        voltage = ADC_Read(0);
        degree = ((unsigned long)voltage - 512) * 90 / 512;
        set_degree(degree);
        SendString("Degree: ");
        sendNumberInt16(degree);
    }
}



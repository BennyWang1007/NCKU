#ifndef _UTILS_H
#define _UTILS_H

#include <xc.h>
#include "global_var.c"

void set_timer2_ms(uint16 ms) {
    uint8 cycles = (uint8)((unsigned long)_XTAL_FREQ / 1000 * ms / 16 / 4 / 4 / TIMER2_POSTSCALER);
    if (cycles != PR2) {
        PR2 = cycles;
    }
}

// able to 4.19 sec (4 * 4 * 16 * 256 / 125000 * timer2_postscaler=8)
void set_timer2_sec(uint16 sec) {
    uint8 cycles = (uint8)((unsigned long)_XTAL_FREQ * (unsigned long)sec / 16 / 4 / 4 / TIMER2_POSTSCALER);
    if (cycles != PR2) {
        PR2 = cycles;
    }
}

void init_timer2() {
    PIR1bits.TMR2IF = 0;    // Clear flag of timer2
    IPR1bits.TMR2IP = 1;    // Set enable bit of timer2
    PIE1bits.TMR2IE = 1;    // Set enable bit of timer2 interrupt
    // T2CON = 0b01111111;  // 7: not implement; 6-3: postscaler, 2: enable, 1-0: prescaler
    T2CON |= 0b01111100;  // 7: not implement; 6-3: postscaler, 2: enable, 1-0: prescaler
}

void set_degree(int16 degree) {
    uint16 value = ((uint16)190 * (uint16)(degree + 90) / 18 + 500) / 32;
    CCPR1L = (uint8)(value / 4);
    CCP1CONbits.DC1B = value % 4;
}

void set_LED_display(uint8 value) {
    LATA = value * 2;
}

uint8 get_LED_display() {
    return LATA / 2;
}


void input_test() {
    char *input = GetString();
    while (1) {
        if (input[0] != '\0') {
            SendString("input = ");
            SendString(input);
            SendString("\r\n");
            ClearBuffer();
            clear_input_buffer();
            return;
        }
    }
}

void send_number_test() {
    while (1) {
        int8 num = get_int8_input();
        SendString("num = ");
        sendNumberInt8(num);
        sendNumberInt16((int)(num));

        int16 num2 = get_int16_input();
        SendString("num2 = ");
        sendNumberInt8((int8)num2);
        sendNumberInt16(num2);
    }
}

#endif  /* _UTILS_H */

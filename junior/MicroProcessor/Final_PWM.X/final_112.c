
#include <stdlib.h>
#include "stdio.h"
#include "string.h"

#include "setting_hardaware/setting.h"
#include "utils.c"

// #define RB0_PRESSED (PORTBbits.RB0 == 0)
// #define _XTAL_FREQ 125000

// typedef unsigned char uint8;
// typedef char int8;
// typedef unsigned int uint16;
// typedef int int16;

void final_112_1A() {
    int8 prev_state = 9, cur_state = 0;
    int16 voltage;
    int16 acc = 0;
    int count = 0;

    set_timer2_ms(500);
    init_timer2();

    while (1) {
        voltage = ADC_Read(0);
        if (voltage > 682) {
            cur_state = 0;
            set_timer2_ms(500);
        } else if (voltage > 341) {
            cur_state = 1;
            set_timer2_ms(250);
        } else {
            cur_state = 2;
            set_timer2_ms(125);
        }

        if (cur_state != prev_state) {
            acc_1A = 0;
            prev_state = cur_state;
        }
    }
}


void final_112_1A_int() {
    if (PIR1bits.TMR2IF) {
        PIR1bits.TMR2IF = 0;
        acc_1A += 1;
        if (LATA & 0b10) LATA &= 0b11111101;
        else LATA |= 0b10;
        // sprintf(msg_1A, "State_%d count = %d\r\n", cur_state + 1, acc_1A); // 17-18 words
        // SendString(msg_1A); // too slow
        // expend for efficiency
        UART_Write('S');
        UART_Write('t');
        UART_Write('a');
        UART_Write('t');
        UART_Write('e');
        UART_Write('_');
        UART_Write((uint8)cur_state + 1 + '0');
        UART_Write(' ');
        UART_Write('c');
        UART_Write('o');
        UART_Write('u');
        UART_Write('n');
        UART_Write('t');
        UART_Write(' ');
        UART_Write('=');
        UART_Write(' ');
        sendNumberInt16(acc_1A);
    }
}


void final_112_2A() {
    cur_state = 0;
    int cur_angle = 0;

    while (1) {
        switch (cur_state) {
        case 0:
            // SendString("Mode 1\r\n");
            if (cur_angle == -45) cur_angle = 45;
            else cur_angle = -45;
            break;

        case 1:
            // SendString("Mode 2\r\n");
            if (cur_angle == -90) cur_angle = 90;
            else cur_angle = -90;
            break;
        }
        set_degree((int16)cur_angle);
        __delay_ms(200);
    }
}


void final_112_2A_int() {
    if (INTCONbits.INT0IF == 1){
        cur_state = (cur_state + 1) & 0x1;
        SendString("Goto mode ");
        sendNumberInt16(cur_state + 1);
        __delay_ms(100);
        INTCONbits.INT0IF = 0;
    }
}

int final_112_2b_mode;

void final_112_2B() {
    final_112_2b_mode = 0;
    int16 cur_angle = -90;
    char str[20];
    while (1) {
        strcpy(str, GetString());
        if (strcmp(str, "mode1") == 0) {
            SendString("into mode 1\r\n");
            ClearBuffer();
            final_112_2b_mode = 0;
        }
        if (strcmp(str, "mode2") == 0) {
            SendString("into mode 2\r\n");
            ClearBuffer();
            final_112_2b_mode = 1;
        }

        if (final_112_2b_mode == 1) {
            if (cur_angle == -90) {
                cur_angle = 90;
                for (int degree = 90; degree >= -90; degree -= 1) {
                    set_degree(degree);
                    __delay_ms(15);
                }
            } else {
                cur_angle = -90;
                set_degree(cur_angle);
                __delay_ms(1000);
            }
        } else {
            if (cur_angle == -90) cur_angle = 90;
            else cur_angle = -90;
            set_degree(cur_angle);
            __delay_ms(1000);
        }
    }
}


void final_112_3A() {
    int16 voltage, degree;
    while (1) {
        voltage = ADC_Read(0);
        SendString("Voltage: ");
        sendNumberInt16(voltage);

        // map 0-1023 to degree 0-90
        // degree = (int16)voltage * 90 / 1023;
        degree = (uint16)voltage * 45 / 512;
        SendString("Degree: ");
        sendNumberInt16(degree);
        set_degree(degree);
    }
}


void final_112_3B() {
    int16 voltage, prev_voltage;
    int16 x = 100;
    char input[20];
    while (1) {
        voltage = ADC_Read(0);
        strcpy(input, GetString());

        if (input[0] != '\0') {
            x = atoi(input);
            SendString("x = ");
            sendNumberInt16(x);
            SendString("Voltage / x = ");
            sendNumberInt16(voltage / x);
            clear_input_buffer();
        }

        // make it a +-30 tolerance
        if (abs(voltage - prev_voltage) > 30) {
            SendString("Voltage / x = ");
            sendNumberInt16(voltage / x);
            prev_voltage = voltage;
            continue;
        }
    }
}
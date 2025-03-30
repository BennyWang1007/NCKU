#include "setting_hardaware/setting.h"
#include <stdlib.h>
#include "stdio.h"
#include "string.h"

#include "utils.c"


void final_113_1A() {
    set_timer2_ms(250);
    init_timer2();
    set_LED_display(1);
    while (1);
}

int8 final_113_1A_state = 0;

void final_113_1A_int() {
    if (PIR1bits.TMR2IF) {
        PIR1bits.TMR2IF = 0;
        timer2_postscaler_counter++;
        if (timer2_postscaler_counter < TIMER2_POSTSCALER) return;
        timer2_postscaler_counter = 0;

        int8 a = get_LED_display();
        if (a == 1) {
            set_LED_display(2);
        } else if (a == 2) {
            set_LED_display(4);
        } else {
            set_LED_display(1);
        }
    } else if (INTCONbits.INT0IF) {
        INTCONbits.INT0IF = 0;
        final_113_1A_state += 1;
        if (final_113_1A_state == 3) {
            final_113_1A_state = 0;
        }
        switch (final_113_1A_state) {
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
int8 final_113_1B_state = 0;
int8 final_113_1B_start = 8;

void final_113_1B() {
    char *input = GetString();
    while (1) {
        if (input[0] != '\0') {
            final_113_1B_state = input[0] - '0';
            final_113_1B_start = input[2] - '0';
            set_LED_display(final_113_1B_start);
            ClearBuffer();
            clear_input_buffer();
            break;
        }
    }
    if (final_113_1B_state == 1) {
        set_timer2_ms(250);
    } else if (final_113_1B_state == 2) {
        set_timer2_ms(500);
    } else {
        set_timer2_ms(1000);
    }
    init_timer2();

    while (1) {
        if (input[0] != '\0') {
            final_113_1B_state = input[0] - '0';
            final_113_1B_start = input[2] - '0';
            set_LED_display(final_113_1B_start);
            ClearBuffer();
            clear_input_buffer();
            if (final_113_1B_state == 1) {
                set_timer2_ms(250);
            } else if (final_113_1B_state == 2) {
                set_timer2_ms(500);
            } else {
                set_timer2_ms(1000);
            }
        }
    }
}


void final_113_1B_int() {
    if (PIR1bits.TMR2IF) {
        PIR1bits.TMR2IF = 0;
        timer2_postscaler_counter++;
        if (timer2_postscaler_counter < TIMER2_POSTSCALER) return;
        timer2_postscaler_counter = 0;
        int8 a = get_LED_display();
        if (a > 0) set_LED_display(a - 1);
    }
}


int8 final_113_2A_mode = 1;
int8 final_113_2A_dir = 0;
int16 final_113_2A_degree = 90;
int16 final_113_2A_degree_start = 90;
int16 final_113_2A_degree_end = -90;

void final_113_2A() {
    final_113_2A_mode = 1;
    final_113_2A_dir = 0;
    final_113_2A_degree = 90;
    final_113_2A_degree_start = 90;
    final_113_2A_degree_end = -90;
    while (1) {
        if (final_113_2A_dir == 0) {
            if (final_113_2A_degree > final_113_2A_degree_end) {
                final_113_2A_degree -= 5;
            } else {
                final_113_2A_dir = 1;
            }
        } else {
            if (final_113_2A_degree < final_113_2A_degree_start) {
                final_113_2A_degree += 5;
            } else {
                final_113_2A_dir = 0;
            }
        }
        set_degree(final_113_2A_degree);
        sendNumberInt16(final_113_2A_degree);
        __delay_ms(10);
    }
}


void final_113_2A_int() {
    if (INTCONbits.INT0IF) {
        // SendString("INT0\r\n");
        INTCONbits.INT0IF = 0;
        if (final_113_2A_mode == 1) {
            final_113_2A_mode = 0;
            final_113_2A_degree = 90;
            final_113_2A_degree_start = 90;
            final_113_2A_degree_end = -90;
        } else {
            final_113_2A_mode = 1;
            final_113_2A_degree = 90;
            final_113_2A_degree_start = 90;
            final_113_2A_degree_end = 0;
        }
    }
}

int8 final_113_2B_dir = 0;
int16 final_113_2B_phase = 0;
int16 final_113_2B_degree = 90;

void final_113_2B() {
    final_113_2B_degree = get_int16_input();
    set_degree(final_113_2B_degree);
    while (1);
}

void final_113_2B_add_degree(int16 degree) {
    int16 next_degree;
    if (final_113_2B_dir == 0) next_degree = final_113_2B_degree + degree;
    else next_degree = final_113_2B_degree - degree;
    if (next_degree > 90) {
        sendNumberInt16(90);
        set_degree(90);
        next_degree = 90 - (next_degree - 90);
        final_113_2B_dir = 1;
        __delay_ms(500);
    } else if (next_degree < -90) {
        sendNumberInt16(-90);
        set_degree(-90);
        next_degree = -90 - (next_degree + 90);
        final_113_2B_dir = 0;
        __delay_ms(500);
    }
    final_113_2B_degree = next_degree;
    set_degree(final_113_2B_degree);
    sendNumberInt16(final_113_2B_degree);
}

void final_113_2B_int() {
    if (INTCONbits.INT0IF) {
        INTCONbits.INT0IF = 0;
        SendString("Phase ");
        sendNumberInt16(final_113_2B_phase);
        if (final_113_2B_phase == 0) {
            final_113_2B_add_degree(45);
        } else if (final_113_2B_phase == 1) {
            final_113_2B_add_degree(90);
        } else if (final_113_2B_phase == 2) {
            final_113_2B_add_degree(180);
        }
        final_113_2B_phase++;
        if (final_113_2B_phase == 3) final_113_2B_phase = 0;
    }
}

int8 final_113_3A_phase = 0;

// int8 final_113_3A_arr[7] = {7, 6, 4, 0, 1, 3, 7};
int8 final_113_3A_arr[7] = {7, 3, 1, 0, 4, 6, 7};

void final_113_3A() {
    final_113_3A_phase = 0;
    int16 voltage;
    while (1) {
        voltage = 1024 - ADC_Read(0);
        int16 phase = voltage / 147;
        set_LED_display(final_113_3A_arr[phase]);
    }
}

int8 final_113_3B_offset = 0;
int8 final_113_3B_prev_phase = 0;
// char final_113_3B_arr[7][4] = {"LLL\0", "DLL\0", "DDL\0", "DDD\0", "LDD\0", "LLD\0", "LLL\0"};
char final_113_3B_arr[7][4] = {"LLL\0", "LLD\0", "LDD\0", "DDD\0", "DDL\0", "DLL\0", "LLL\0"};

void final_113_3B() {
    final_113_3A_phase = 0;
    final_113_3B_prev_phase = 0;
    final_113_3B_offset = 0;
    int16 voltage;
    int16 phase, ab_phase;
    char *input = GetString();
    while (1) {
        if (input[0] != '\0') {
            long long x = atoll(input);
            final_113_3B_offset += x % 7;
            SendString("Input mod 7: ");
            sendNumberInt8(final_113_3B_offset);
            ClearBuffer();
            clear_input_buffer();
            final_113_3B_prev_phase = -1;
        }
        voltage = 1024 - ADC_Read(0);
        phase = voltage / 147;
        ab_phase = (phase + final_113_3B_offset) % 7;
        if (phase != final_113_3B_prev_phase) {
            set_LED_display(final_113_3A_arr[ab_phase]);
            SendString(final_113_3B_arr[ab_phase]);
            SendString(" ");
            sendNumberInt16(ab_phase+1);
            final_113_3B_prev_phase = phase;
        }
    }
}
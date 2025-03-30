#include "setting_hardaware/setting.h"
#include <stdlib.h>
#include "stdio.h"
#include "string.h"
// using namespace std;

char str[20];

#define ADVANCED
#define BONUS

#define _XTAL_FREQ 4000000

#define RB0_PRESSED (PORTBbits.RB0 == 0)
#define TERMINATE_INPUT (str[0] == 'q')


void lab10_advanced() {
    while(1) {
        strcpy(str, GetString()); // TODO : GetString() in uart.c
        if(str[0] >= '0' && str[0] <= '9'){ // Mode1
            LATA = (str[0] - '0') << 1;
        } else if (TERMINATE_INPUT) {
            ClearBuffer();
            SendString("\r\nAdvanced Terminated\r\n");
            break;
        }
        ClearBuffer();
    }
}

void lab10_bonus() {
    int target = 0;
    while(1) {
        if (RB0_PRESSED) {
            target = 0;
            LATA = 0;
        }
        strcpy(str, GetString()); // TODO : GetString() in uart.c
        if(str[0] >= '0' && str[0] <= '9'){ // Mode1
            target = (str[0] - '0');
        } else if (TERMINATE_INPUT) {
            ClearBuffer();
            SendString("\r\nBonus Terminated\r\n");
            break;
        }
        if (LATA <= ((target - 1) << 1)) {
            LATA += 2;
        } else {
            LATA = 0;
        }
        ClearBuffer();
        __delay_ms(500);
    }
}


void main(void) {

    SYSTEM_Initialize();

    // init LED, set RA1-5 as output
    TRISA = 0b00000001;
    LATA = 0;

    // init botton, set RB0 as input
    TRISB = 0X01;

    // send instruction
    UART_Write_Text("Please enter mode(m1/m2):\r\n");

    while(1) {
        strcpy(str, GetString()); // TODO : GetString() in uart.c
        if(str[0]=='m' && str[1]=='1'){ // Mode1
            ClearBuffer();
            SendString("\r\nMode1\n");
            lab10_advanced();
        }
        else if(str[0]=='m' && str[1]=='2'){ // Mode2
            ClearBuffer();
            SendString("\r\nMode2\r\n");
            lab10_bonus();
        }
    }

    return;
}

void __interrupt(high_priority) Hi_ISR(void) {

}
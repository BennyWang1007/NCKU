#include <xc.h>
#include <string.h>
#include "stdio.h"  // for sprintf

#define MY_STRLEN_LIMIT 15

char mystring[20];
int lenStr = 0;

char my_input[20];
int lenInput = 0;

char send_buf[20];

void UART_Initialize() {

    /*  TODObasic
        Serial Setting
        1.   Setting Baud rate
        2.   choose sync/async mode
        3.   enable Serial port (configures RX/DT and TX/CK pins as serial port pins)
        3.5  enable Tx, Rx Interrupt(optional)
        4.   Enable Tx & RX
    */

    TRISCbits.TRISC6 = 1;
    TRISCbits.TRISC7 = 1;

    //  Setting baud rate
    if (OSCCONbits.IRCF == 0b110) {         // 4 MHz
        TXSTAbits.SYNC = 0;                 // clear to enable the asynchronous serial port
        BAUDCONbits.BRG16 = 0;              // determine using 8/16 bit (SPBRG / SPBRGH + SPBRG)
        TXSTAbits.BRGH = 0;                 // high baud rate select bit: Asynchronous mode: 1 = High speed, 0 = Low speed (Synchronous mode: Unused)
        SPBRG = 51;                         // controls the period of a free-running timer (check data sheet)
    }
    else if (OSCCONbits.IRCF == 0b001) {    // 125kHz
        TXSTAbits.SYNC = 0;                 // clear to enable the asynchronous serial port
        BAUDCONbits.BRG16 = 1;              // determine using 8/16 bit (SPBRG / SPBRGH + SPBRG)
        TXSTAbits.BRGH = 0;                 // high baud rate select bit: Asynchronous mode: 1 = High speed, 0 = Low speed (Synchronous mode: Unused)
//        SPBRG = 26;
        SPBRG = 25;
    }

    // Serial enable
    RCSTAbits.SPEN = 1;             // set to configure Uart pin    // TXSTA? // clear to enable the asynchronous serial port
    PIR1bits.TXIF = 1;              // set when TXREG is empty
    PIR1bits.RCIF = 0;              // set when reception completes. (ReCieve Int. Flag)
    TXSTAbits.TXEN = 1;             // enable the transmission
    RCSTAbits.CREN = 1;             // enable the reception
    PIE1bits.TXIE = 0;              // set if interrupt is desired
    IPR1bits.TXIP = 0;              // set if high priority
    PIE1bits.RCIE = 1;              // set if interrupt is desired
    IPR1bits.RCIP = 0;              // set if high priority

}

void UART_Write(unsigned char data)  // Output on Terminal
{
    while (!TXSTAbits.TRMT);
    TXREG = data;              //write to TXREG will send data
}


void UART_Write_Text(char* text) { // Output on Terminal, limit:15 chars
    // for (int i = 0; text[i] != '\0'; i++)
    //     UART_Write(text[i]);
    while (*text)
        UART_Write(*text++);
}

void ClearBuffer(void) {
    for (int i = 0; i < MY_STRLEN_LIMIT; i++)
        mystring[i] = '\0';
    lenStr = 0;
}

void clear_input_buffer(void) {
    for(int i = 0; i < MY_STRLEN_LIMIT; i++)
        my_input[i] = '\0';
    lenInput = 0;
}

void record_input(void) {
    for(int i = 0; i < lenStr; i++)
        my_input[i] = mystring[i];
    lenInput = lenStr;
}


void SendString(char *send_str) {
    int loop_count = strlen(send_str) / MY_STRLEN_LIMIT;
    for (int i = 0; i <= loop_count; i++) {
        char tmp[MY_STRLEN_LIMIT + 1];
        for (int k = 0; k < MY_STRLEN_LIMIT; k++) {
            tmp[k] = send_str[i * MY_STRLEN_LIMIT + k];
        }
        tmp[MY_STRLEN_LIMIT] = '\0';
        UART_Write_Text(tmp);
    }
}

void sendNumberInt16(int num) {
    if (num == 0) {
        UART_Write('0');
        UART_Write('\r');
        UART_Write('\n');
        return;
    }

    char *p = send_buf;
    if (num < 0) {
        *p++ = '-';
        num = -num;
    }
    while (num > 0) {
        *p++ = num % 10 + '0';
        num /= 10;
    }
    *p = '\0';

    // Reverse the string
    char *start = send_buf;
    char *end = p - 1;
    if (*start == '-') start++;
    while (start < end) {
        char temp = *start;
        *start = *end;
        *end = temp;
        start++;
        end--;
    }

    SendString(send_buf);
    UART_Write('\r');
    UART_Write('\n');
}

void sendNumberInt8(char num) {
    int num2 = (int)(num);
    sendNumberInt16(num2);
}


void MyusartRead(void) {
    /* TODObasic: try to use UART_Write to finish this function */
    while (!PIR1bits.RCIF);
    mystring[lenStr] = RCREG;

    // if is backspace
    if (mystring[lenStr] == '\b') {
        if (lenStr > 0) {
            mystring[lenStr] = '\0';
            lenStr--;
            SendString("lenStr: ");
            sendNumberInt8(lenStr);
        }
        UART_Write(0x08);
        return;
    }
    // if is enter
    if (mystring[lenStr] == '\r') {
        mystring[lenStr] = '\0';
        record_input();
        lenStr = 0;
        UART_Write('\r');
        UART_Write('\n');
        return;
    }
    UART_Write(RCREG);
    lenStr++;
    if (lenStr == MY_STRLEN_LIMIT)
        lenStr = 0;
    return;
}

char *GetString(){
    // return mystring;
    return my_input;
}



// void interrupt low_priority Lo_ISR(void)
void __interrupt(low_priority)  Lo_ISR(void) {
    if (RCIF) {
        if (RCSTAbits.OERR) {
            CREN = 0;
            Nop();
            CREN = 1;
        }
        MyusartRead();
    }
    return;
}
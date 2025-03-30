#include <xc.h>
#include <string.h>
    //setting TX/RX

char mystring[20];
int lenStr = 0;

void UART_Initialize(void) {
           
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
    
    // Setting baud rate
    TXSTAbits.SYNC = 0;             // clear to enable the asynchronous serial port
    BAUDCONbits.BRG16 = 0;          // determine using 8/16 bit (SPBRG / SPBRGH + SPBRG)  
    TXSTAbits.BRGH = 0;             // high baud rate select bit: Asynchronous mode: 1 = High speed, 0 = Low speed (Synchronous mode: Unused)
    SPBRG = 51;                     // controls the period of a free-running timer (check data sheet)
    
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

void UART_Write(unsigned char data) { // Output on Terminal
    while(!TXSTAbits.TRMT);
    TXREG = data; // write to TXREG will send data 
}


void UART_Write_Text(char* text) { // Output on Terminal, limit:10 chars
    for(int i = 0; text[i] != '\0'; i++)
        UART_Write(text[i]);
}

void ClearBuffer(void) {
    for(int i = 0; i < 10 ; i++)
        mystring[i] = '\0';
    lenStr = 0;
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
        }
        UART_Write(0x08);
        return;
    }
    // if is enter
    if (mystring[lenStr] == '\r') {
        mystring[lenStr] = '\0';
        lenStr = 0;
        return;
    }
    UART_Write(RCREG);
    lenStr++;
    if(lenStr == 10)
        lenStr = 0;
    return ;
}

char *GetString() {
    return mystring;
}


void SendString(char *send_str) {
    for (int i = 0; i <= strlen(send_str) / 10; i++) {
        char tmp[10];
        for (int k = 0; k < 10; k++) {
            tmp[k] = send_str[i * 10 + k];
        }
        UART_Write_Text(tmp);
    }
}

void __interrupt(low_priority) Lo_ISR(void) {
    if(RCIF) {
        if(RCSTAbits.OERR) {
            CREN = 0;
            Nop();
            CREN = 1;
        }
        MyusartRead();
    }
    // process other interrupt sources here, if required
    return;
}
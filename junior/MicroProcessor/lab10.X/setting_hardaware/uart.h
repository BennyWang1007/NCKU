#ifndef _UART_H
#define _UART_H

void UART_Initialize(void);
char * GetString(void);
void UART_Write(unsigned char data);
void UART_Write_Text(char* text);
void ClearBuffer(void);
void MyusartRead(void);
void SendString(char *send_str);

#endif
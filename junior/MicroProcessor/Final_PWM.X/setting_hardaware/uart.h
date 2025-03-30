#ifndef _UART_H
#define _UART_H
      
void UART_Initialize(void);
char *GetString(void);
void UART_Write(unsigned char data);
void UART_Write_Text(char* text);
void ClearBuffer(void);
void MyusartRead(void);
void SendString(char *send_str);
void sendNumberInt8(char num);
void sendNumberInt16(int num);
void record_input(void);
void clear_input_buffer(void);

#endif  /* _UART_H */
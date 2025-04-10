/* 
 * File:   threewire.h
 * Author: laptop
 *
 * Created on 2025?1?13?, ?? 4:29
 */

#ifndef THREEWIRE_H
#define	THREEWIRE_H

#ifdef	__cplusplus
extern "C" {
#endif


#define IO_PIN PORTCbits.RC4
#define IO_TRIS TRISCbits.TRISC4
#define CLK_PIN PORTCbits.RC3
#define CLK_TRIS TRISCbits.TRISC3
#define CE_PIN PORTCbits.RC5
#define CE_TRIS TRISCbits.TRISC5
#define OUTPUT 0
#define INPUT 1

#include "../constants.c"
    
extern const uint8_t THREEWIRE_READFLAG;

void ThreeWire_Init(void);
void ThreeWire_Reset(void);
void ThreeWire_Begin(void);
void ThreeWire_End(void);
void ThreeWire_BeginTransmission(uint8_t);
void ThreeWire_EndTransmission(void);
uint8_t ThreeWire_Read(void);
void ThreeWire_Write(uint8_t, uint8_t);




#ifdef	__cplusplus
}
#endif

#endif	/* THREEWIRE_H */


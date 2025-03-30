/*
 * File:   main.c
 * Author: benny
 *
 * Created on 2024/10/21?, ?? 4:31
 */


#include <xc.h>

//#define BASIC
//#define ADVANCED
#define BONUS

typedef unsigned char uint8;
typedef char int8;
typedef unsigned int uint16;
typedef int int16;

extern uint16 add(uint16 a, uint16 b);
extern uint8 mysqrt(uint16 a);
extern uint16 gcd(uint16 a, uint16 b);
extern uint16 multi_signed(uint8 a, uint8 b);

void main(void) {
#ifdef BASIC
    volatile uint8 result1 = mysqrt(20);
    __nop();
    volatile uint8 result2 = mysqrt(50);
    __nop();
#endif
    
#ifdef ADVANCED
    volatile uint16 result1 = gcd(12321, 65535);
    __nop();
    volatile uint16 result2 = gcd(1200, 180);
    __nop();
#endif

#ifdef BONUS
    volatile uint16 result1 = multi_signed(-20, -4);
    volatile uint16 result2 = multi_signed(-127, 6);
    volatile uint16 result3 = multi_signed(-127, -6);
    volatile uint16 result4 = multi_signed(127, 6);
    volatile uint16 result5 = multi_signed(100, 0);
    __nop();
#endif
    
    return;
}

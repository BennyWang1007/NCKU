#include <xc.h>

void ADC_Initialize(void) {
    ADCON1 = 0x0e;              // Ref vtg is VDD & Configure pin as analog pin 
    
    TRISAbits.RA0 = 1;          // analog input port
    
    ADCON1bits.VCFG0 = 0;
    ADCON1bits.VCFG1 = 0;
    ADCON1bits.PCFG = 0b1110;   // set AN0 as analog input, others as digital
    ADCON0bits.CHS = 0b0000;    // set AN0 as analog input
    ADCON2bits.ACQT = 0b001;    // Tad = 2 us acquisition time, 2Tad = 4 > 2.4
    ADCON0bits.ADON = 1;
    ADCON2bits.ADFM = 0;        // left justified 

    TRISA = 0b00000001;

    ADCON2bits.ADCS = 7;
    ADRESH = 0;                 // Flush ADC output Register
    ADRESL = 0;  
}

int ADC_Read(int channel) {
    int digital;
    
    ADCON0bits.CHS = 0x07;      // Select Channe7
    ADCON0bits.GO = 1;
    ADCON0bits.ADON = 1;
    
    while(ADCON0bits.GO_nDONE == 1);

    digital = (ADRESH * 256) | (ADRESL);
    return (digital);
}

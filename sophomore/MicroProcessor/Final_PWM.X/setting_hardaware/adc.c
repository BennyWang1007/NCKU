#include <xc.h>



void ADC_Initialize(void) {
    ADCON1 = 0x0e;              // Ref vtg is VDD & Configure pin as analog pin
    // ADCON2 = 0x92;

    TRISAbits.RA0 = 1;          // set RA0 as input port

    ADCON1bits.VCFG0 = 0;
    ADCON1bits.VCFG1 = 0;
    ADCON1bits.PCFG = 0b1110;   // set AN0 as analog input, others as digital
    ADCON0bits.CHS = 0b0000;    // set AN0 as analog input
//    ADCON2bits.ADCS = 0b000;  // set to 000 (1Mhz < 2.86Mhz) by the sheet
    ADCON2bits.ACQT = 0b001;    // Tad = 2 us acquisition time?2Tad = 4 > 2.4
    ADCON0bits.ADON = 1;
//    ADCON2bits.ADFM = 0;      // left justified
    ADCON2bits.ADFM = 1;        // right justified, for 10 bit ADC

    TRISA = 0b00000001;

    ADCON2bits.ADCS = 7;
    ADRESH = 0;                 // Flush ADC output Register
    ADRESL = 0;
}

int ADC_Read(int channel) {
    int digital;

    ADCON0bits.CHS = channel; // Select Channe
    ADCON0bits.GO = 1;
    ADCON0bits.ADON = 1;

    while (ADCON0bits.GO_nDONE == 1);

    digital = (ADRESH*256) | (ADRESL);
//    digital = ADRESH;

    return digital;
}
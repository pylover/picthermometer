
// PIC12F675 Configuration Bit Settings

// 'C' source line config statements

// BEGIN CONFIG
#pragma config FOSC =  INTRCIO  // Oscillator Selection bits (RC oscillator: IO function on GP4/OSC2/CLKOUT pin, GP5/OSC1/CLKIN)
#pragma config WDTE =  ON       // Watchdog Timer Enable bit (WDT enabled)
#pragma config PWRTE = ON       // Power-Up Timer Enable bit (PWRT enabled)
#pragma config MCLRE = OFF      // GP3/MCLR pin function select (GP3/MCLR pin function is MCLR)
#pragma config BOREN = ON       // Brown-out Detect Enable bit (BOD enabled)
#pragma config CP = OFF         // Code Protection bit (Program Memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
// END CONFIG

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include "max7219.c"

static short adcvalue = 0;


#define DUAL_SENSOR
#define INTERVAL                300000
#define RATIO                   0.152587
#define OFFSET                  0
#define C(x)                    ((x - OFFSET) * RATIO)
#define DISPLAY_INTENSITY       4


void interrupt isr(void) {
    if (ADIF) {
        adcvalue = ADRESH << 8;
        adcvalue += ADRESL;
        
#ifdef DUAL_SENSOR
        CHS0 = !CHS0;
#endif
        ADIF = 0;
    } 
}

#define TAD     0b01010000


int main() {
#ifdef DUAL_SENSOR
    TRISIO = 0b00010100;        // GP2: IN, GP4: IN
    ANSEL = 0b01011100;         // GP4->AN3, GP2->AN2
#else
    TRISIO = 0b00000100;        // GP2: IN
    ANSEL = 0b01010100;         // GP2->AN2
#endif

    ANSEL |= TAD;
    OPTION_REG = 0b11111111;
    CMCON = 0b00000111;
    ADCON0 = 0b10001001;        // ADON, AN2, VDD
    VRCON = 0b00000000;
    INTCON = 0b11000000;        // GIE,

    ADIE = 1;
    ADIF = 0;
    max7219_init(DISPLAY_INTENSITY);
    GO_nDONE = 1;               // ADC enable
    while (1) {
        if (GO_nDONE == 1){
            continue;
        }
#ifdef DUAL_SENSOR
        displayfloat(!CHS0, C(adcvalue));
#else
        displayfloat(left, C(adcvalue));
        display(right, adcvalue, 0);
#endif
        GO_nDONE = 1;           // ADC enable
        _delaywdt(INTERVAL);
    }
    return 0;
}

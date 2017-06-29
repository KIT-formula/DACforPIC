//     RB2  SPISDO
//     RB4  SPISCK
//     RB5  SPI_CS
// PIC16F88 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FOSC = HS  // Oscillator Selection bits (INTRC oscillator; port I/O function on both RA6/OSC2/CLKO pin and RA7/OSC1/CLKI pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT enabled)
#pragma config MCLRE = ON       // RA5/MCLR/VPP Pin Function Select bit (RA5/MCLR/VPP pin function is MCLR)
#pragma config BOREN = ON       // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF        // Low-Voltage Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EE Memory Code Protection bit (Code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off)
#pragma config CCPMX = RB3      // CCP1 Pin Selection bit (CCP1 function on RB3)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

// CONFIG2
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal External Switchover mode disabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <stdio.h>
#include <xc.h>
#define _XTAL_FREQ 20000000

unsigned int send_data = 0;
int flag = 0,Count = 0;

void main(void){

    OSCCON = 0b00000000;        //inside clock 8MHz

    //Port config
    PORTA  = 0x00;              //PORTA all low
    PORTB  = 0x00;              //PORTB all low
    TRISA  = 0xFF;              //PORTA all INPUT
    TRISB  = 0b00000000;        //PORTB all OUTPUT
    ANSEL  = 0x00;              //AN1-AN6 digital
    
    //SPI config
    SSPSTAT = 0b11000000;       //SPI , MODE_00, SMPEND
    SSPCON  = 0b00100001;       //SSP move approval 
    PORTB   = 0b00100000;       // SS HIGH LDAC LOW
    
    //peripheral interrupt config
    PEIE   = 1;                 // peripheral interrupt approval
    GIE    = 1;                 // all interrupt approval
    
    //Timer config
    TMR2   = 0;                 // Timer2 format
    TMR2IF = 0;                 // Timer2 interrupt flag clear
    TMR2IE = 1;                 // Timer2 interrupt approval
    T2CON  = 0b00000000;        // 488micros intervals
    PR2 = 9;                   // Timer2

    while(1) {
        if(PORTAbits.RA0 == 1){
            T2CON  = 0b00100100;
            while(PORTAbits.RA0);
            T2CON  = 0b00000001;
            PORTB = 0b00001000;          // SS LOW
            SSPBUF = Count >> 8 | 0x30; // MSB data send start 
            while(!SSPSTATbits.BF);         // wait send finish
            SSPBUF = Count;             // LSB data send start
            while(!SSPSTATbits.BF);         // wait send finish
            PORTB = 0b00100000;          // SS HIGH LDAC LOW
            __delay_us(5);
            PORTB = 0b00101000;          // LDAC HIGH
            Count = 0;
        }
    }
}

void interrupt InterTimer(void){
    if(TMR2IF == 1){                 //Timer2 interrupt
            Count += 21;
    }
    TMR2IF = 0;
}

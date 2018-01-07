/* 
 * File:   LedBlink.c
 * Author: Jignesh D. Popat
 *
 * Created on January 7, 2018, 5:27 PM
 */

#include "xc.h"
#include <stdio.h>
#include <stdlib.h>

#define LED1    LATCbits.LATC5      //LED1 Output PIN
#define LED2    LATCbits.LATC4      //LED2 Output PIN

void ConfigCLK(void);
void InitIO(void);
void delay_ms(unsigned int count);


void main()
{
    ConfigCLK();
    NOP();
    InitIO();
    NOP();
	
	LED1 = 0;
	LED2 = 1;
	
	while(1)
    {
		LED1 = ~LED1;
		LED2 = ~LED2;
		delay_ms(1000);
	}
}

void ConfigCLK(void)
{
    OSCCON1 = 0x60;          //HFINTOSC and Clock Devision
    OSCFRQ = 0x03;           //Internal 8MHz
}

void InitIO(void)
{
	ANSELA = 0x00;          //Set all pins as Digital
	ANSELC = 0x00;

	TRISA = 0;              //Set all pins as Output
	TRISC = 0;

    TRISCbits.TRISC5 = 0;   //Define LED1 as an output
    TRISCbits.TRISC4 = 0;   //Define LED2 as an output
    
    LATA = 0;               //all pins output 0 value
	LATC = 0;
}

void delay_ms(unsigned int count)
{
    for(unsigned int i = 0; i < count; i++)
    {
        for(unsigned int j = 0; j < 2000; j++);
    }
}
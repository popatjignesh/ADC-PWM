/* 
 * File:   main.c
 * Author: Jignesh D. Popat
 *
 * Created on December 30, 2017, 3:24 PM
 */

#include "xc.h"
#include <stdio.h>
#include <stdlib.h>

#define SW1     PORTAbits.RA5       //SW1 Input PIN
#define SW2     PORTAbits.RA4       //SW2 Input PIN
#define ADC_IN  PORTCbits.RC4       //ADC Input PIN

#define PWM3    LATAbits.LATA2      //PWM3 Output PIN
#define PWM4    LATCbits.LATC0      //PWM4 Output PIN
#define PWM5    LATCbits.LATC1      //PWM5 Output PIN
#define PWM6    LATCbits.LATC2      //PWM6 Output PIN


void ConfigCLK(void);
void InitIO(void);
void InitADC(void);
void InitPWM3n5(void);
void DisablePWM3n5(void);
void InitPWM4n6(void);
void DisablePWM4n6(void);
void ConfigDutyCycle(void);
void FirePWM(void);

void delay_ms(unsigned int count);

unsigned char temp8;
unsigned char PWMH, PWML;
unsigned char PreviousSW2status,SW2status;

unsigned int temp16;
unsigned int PreviousADC, ADCresult;
int ADCdifference;

void main()
{
    ConfigCLK();
    NOP();
    InitIO();
    NOP();

    ADCresult = 0;
    ADCdifference = 0;
    PWMH = PWML = 0;
    PreviousSW2status = SW2;
    PreviousADC = 0;

    if(SW1)
    {
        InitADC();
        NOP();

        ConfigDutyCycle();
        NOP();

        FirePWM();
        NOP();
    }
    while(1)
    {
        if(SW1)
        {
            SW2status = SW2;

            if(PreviousSW2status != SW2status)
            {
                PreviousSW2status = SW2status;
        
                InitADC();
                NOP();

                ConfigDutyCycle();
                NOP();
                
                FirePWM();
                NOP();
            }
            
            InitADC();
            NOP();
            ADCdifference = abs(ADCresult - PreviousADC);
       
            if(ADCdifference > 9)
            {
                PreviousADC = ADCresult;

                ConfigDutyCycle();
                NOP();
                
                FirePWM();
                NOP();
            }
        }
		else
		{
			DisablePWM3n5();
			NOP();
			DisablePWM4n6();
			NOP();
		}
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
    
    TRISAbits.TRISA5 = 1;   //Define SW1 as an input
    TRISAbits.TRISA4 = 1;   //Define SW2 as an input
    TRISCbits.TRISC4 = 1;   //Define ADC as an input

	TRISAbits.TRISA2 = 0;   //Define PWM3 as an output
	TRISCbits.TRISC0 = 0;   //Define PWM4 as an output
    TRISCbits.TRISC1 = 0;   //Define PWM5 as an output
    TRISCbits.TRISC2 = 0;   //Define PWM6 as an output
    
    LATA = 0;               //all pins output 0 value
	LATC = 0;
}

void InitADC(void)
{
	TRISCbits.TRISC4 = 1;			//ADC Input PIN Enabled
	ANSELCbits.ANSC4 = 1;			//Configure RC4 as an Analog channel
	ADCON1bits.ADFM = 0;			//Get Left justified result
	ADCON1bits.ADCS = 0b001;		//ADC clock as Fosc/8
	ADCON1bits.ADPREF = 0b00;		//Vdd as +Vref
	ADCON0bits.CHS = 0b010100;      //RC4 as an Analog Channel
	ADCON0bits.ADON = 1;			//ADC is enabled
	//Generate 1uSec Delay here
	ADCON0bits.GOnDONE = 1;			//Start ADC
	while(ADCON0bits.GOnDONE);		//Wait till ADC Conversation complete
	ADCresult = ADRES;				//Get ADC Result
}

void InitPWM3n5(void)
{
    TRISAbits.TRISA2 = 1;           //PWM3 Output PIN disabled
    TRISCbits.TRISC1 = 1;           //PWM5 Output PIN disabled

    PWM3CONbits.PWM3POL = 0;        //For PWM3 active high signal
    PWM5CONbits.PWM5POL = 0;        //For PWM5 active high signal

    PR2 = 0xFF;                     //Generate PWM with 19.61 KHz

    PWM3DCH = PWM5DCH = PWMH;       //Load Duty Cycle according to ADC
    PWM3DCL = PWM5DCL = PWML; 

    PIR4bits.TMR2IF = 0;            //Clear Timer2 Interrupt Flag
    T2CONbits.CKPS = 0x01;          //Timer Pre Scalar as 1 for 19.61KHz
    T2CONbits.ON = 1;               //Timer2 ON
    while(PIR4bits.TMR2IF == 0);    //Wait till Timer2 Interrupt Flag sets

    TRISAbits.TRISA2 = 0;           //PWM3 Output PIN enabled
    TRISCbits.TRISC1 = 0;           //PWM5 Output PIN enabled
    RA2PPS = 0x0B;                  //Map PWM3 on RA2 Pin
    RC1PPS = 0x0D;                  //Map PWM5 on RC1 Pin

    PWM3CONbits.PWM3EN = 1;         //Enable PWM3
    PWM5CONbits.PWM5EN = 1;         //Enable PWM5
}

void DisablePWM3n5(void)
{
    TRISAbits.TRISA2 = 1;           //PWM3 Output PIN disabled
    TRISCbits.TRISC1 = 1;           //PWM5 Output PIN disabled
    
    PWM3CONbits.PWM3EN = 0;         //Disable PWM3
    PWM5CONbits.PWM5EN = 0;         //Disable PWM5
}

void InitPWM4n6(void)
{
    TRISCbits.TRISC0 = 1;           //PWM4 Output PIN disabled
    TRISCbits.TRISC2 = 1;           //PWM6 Output PIN disabled

    PWM4CONbits.PWM4POL = 0;        //For PWM4 active high signal
    PWM6CONbits.PWM6POL = 0;        //For PWM6 active high signal

    PR2 = 0xFF;                     //Generate PWM with 19.61 KHz

    PWM4DCH = PWM6DCH = PWMH;       //Load Duty Cycle according to ADC
    PWM4DCL = PWM6DCL = PWML; 

    PIR4bits.TMR2IF = 0;            //Clear Timer2 Interrupt Flag
    T2CONbits.CKPS = 0x01;          //Timer Pre Scalar as 1 for 19.61KHz
    T2CONbits.ON = 1;               //Timer2 ON
    while(PIR4bits.TMR2IF == 0);    //Wait till Timer2 Interrupt Flag sets

    TRISCbits.TRISC0 = 0;           //PWM4 Output PIN enabled
    TRISCbits.TRISC2 = 0;           //PWM6 Output PIN enabled
    RC0PPS = 0x0C;                  //Map PWM4 on RC0 Pin
    RC2PPS = 0x0E;                  //Map PWM6 on RC2 Pin

    PWM4CONbits.PWM4EN = 1;         //Enable PWM4
    PWM6CONbits.PWM6EN = 1;         //Enable PWM6
}

void DisablePWM4n6(void)
{
    TRISCbits.TRISC0 = 1;           //PWM4 Output PIN disabled
    TRISCbits.TRISC2 = 1;           //PWM6 Output PIN disabled
    
    PWM4CONbits.PWM4EN = 0;         //Disable PWM4
    PWM6CONbits.PWM6EN = 0;         //Disable PWM6
}


void ConfigDutyCycle(void)
{
    temp16 = ADCresult;
    temp16 = temp16 & 0xFF00;
    temp16 = temp16>>8;
    temp16 = temp16 & 0x00FF;
    PWMH = (unsigned char)temp16;

    temp16 = ADCresult;
    temp16 = temp16 & 0x00FF;
    PWML = (unsigned char)temp16;
}

void FirePWM(void)
{
    if(SW2)
    {
        DisablePWM4n6();
        InitPWM3n5();
    }
    else
    {
        DisablePWM3n5();
        InitPWM4n6();
    }
}

void delay_ms(unsigned int count)
{

}


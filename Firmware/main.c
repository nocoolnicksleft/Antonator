

#include <p18cxxx.h>
#include <delays.h>
#include <timers.h>
#include <stdio.h>
#include <string.h>

#pragma config FOSC = IRC, CPUDIV = NOCLKDIV, PLLEN = OFF
#pragma config FCMEN = OFF, BOREN = OFF
#pragma config WDTEN = OFF, MCLRE = ON, LVP = OFF

//#define DEBUG_GENERAL 1


#define DEBUG_LED PORTBbits.RB6

//#define BLINKER_A PORTCbits.RC0
//#define BLINKER_B PORTCbits.RC1

#define BLINKER_A LATCbits.LATC1
#define BLINKER_B LATCbits.LATC2


#define POWER_HOLD PORTCbits.RC2

/* ***************************** */
/*                               */
/* ***************************** */
void high_isr(void);

/**********************************************************
/ Global
/**********************************************************/

unsigned char timeout50msec = 0; // TODO: Make bit variable
unsigned char timeout1sec = 0; // TODO: Make bit variable
unsigned char timeout500msec = 0; // TODO: Make bit variable

#define TIMEOUT_50MSEC_START 5
#define TIMEOUT_500MSEC_START 50
#define TIMEOUT_1SEC_START 100

unsigned char timer50msec = TIMEOUT_50MSEC_START;
unsigned char timer500msec = TIMEOUT_500MSEC_START;
unsigned char timer1sec = TIMEOUT_1SEC_START;


/**********************************************************
/ Interrupt High
/**********************************************************/
#pragma code high_vector=0x08
void interrupt_at_high_vector(void)
{
_asm GOTO high_isr _endasm
}


#pragma code /* return to the default code section */
#pragma interrupt high_isr
void high_isr (void)
{

	if (INTCONbits.TMR0IF) { // TIMER0: 

		// TMR0H = 0x65; // 20msec
		TMR0H = 0xB3; // 10msec
        TMR0L = 0x00;

		INTCONbits.TMR0IF = 0; // CLEAR INTERRUPT FLAG
		
	    if (!timer50msec) {
			timeout50msec = 1;
			timer50msec = TIMEOUT_50MSEC_START;
		} else timer50msec--;
 
	    if (!timer500msec) {
			timeout500msec = 1;
			timer500msec = TIMEOUT_500MSEC_START;
		} else timer500msec--;

	    if (!timer1sec) {
			timeout1sec = 1;
			timer1sec = TIMEOUT_1SEC_START;
		} else timer1sec--;
	
	}

}

/**********************************************************
/ MAIN
/**********************************************************/
void main (void)
{

    int blinkstate = 0;

    int shutdowncountdown = 1200;

	OSCCON = 0x70;  // bit 6-4 IRCF<2:0>: Internal Oscillator Frequency Select bits = 16MHZ

	ANSEL = 0x00; // All Pins digital
	ANSELH = 0x00; // All Pins digital
	ADCON0 = 0x00; // ADC Off

	TRISA = 0; // PORT A ALL OUTPUT
	TRISB = 0; // PORT B ALL OUTPUT
	TRISC = 0; // PORT C ALL OUTPUT

	BLINKER_A = 0;
	BLINKER_B = 0;

	POWER_HOLD = 1;

  	INTCONbits.TMR0IE = 1;   // TMR0 Overflow Interrupt Enable bit

	OpenTimer0( 
		TIMER_INT_ON &
		T0_16BIT &
		T0_SOURCE_INT &
		T0_PS_1_1 );

	INTCONbits.GIE = 1; // General Interrupt Enable
	INTCONbits.GIEH = 1; // High Priority Interrupt Enable
	INTCONbits.GIEL = 0; // Low Priority Interrupt Disable

  	while (1) {

		if (timeout500msec) {
			timeout500msec = 0;

			if (blinkstate) {
				#ifdef DEBUG_GENERAL
					DEBUG_LED = 1;
                #endif
				BLINKER_A = 0;
				BLINKER_B = 1;
				blinkstate = 0;
			} else {
				#ifdef DEBUG_GENERAL
					DEBUG_LED = 0;
                #endif
				BLINKER_A = 1;
				BLINKER_B = 0;
				blinkstate = 1;
			}
/*
			shutdowncountdown--;

			if (shutdowncountdown <= 0) {
				POWER_HOLD = 0;
			}
*/
		}

	}

}



#include <htc.h>
#include	"adc.h"


__CONFIG(1,FOSC_INTOSC & WDTE_OFF & MCLRE_OFF & PWRTE_ON & CP_OFF & IESO_OFF & FCMEN_OFF);	// Setup the configuration word 
__CONFIG(2,PLLEN_OFF & BORV_HI & LVP_OFF & STVREN_OFF);	// Setup the configuration word 2

#ifndef _XTAL_FREQ
 // Unless already defined assume 4MHz system frequency
 // This definition is required to calibrate __delay_us() and __delay_ms()
 #define _XTAL_FREQ 32000000
#endif


#define OUT_PIN RA5					// pin2 (il 3 è in adc ossia pin2...)
#define DISCHARGE_PIN RA0		// 


/* Sample code to set up the A2D module */
void init_a2d(void){

	ADCON0=0b00000000;	// select 
	ADCON1=0b00010000;	// select left justify result. A/D port configuration 0; Fosc / 8
	ADON=1;		// turn on the A2D conversion module
	}

/* Return an 8 bit result */
unsigned char read_a2d(unsigned char channel){

	channel &= 0x07;	// truncate channel to 3 bits
	ADCON0 &= 0x83;	// clear current channel select
	ADCON0 |= (channel << 2);	// apply the new channel select
	ADGO=1;	// initiate conversion on the selected channel
	while(ADGO)
		;
	return ADRESH;	// return 8 MSB of the result
	}

unsigned char adc_pin2,adc_pin6,vref_pin5;
const unsigned char VREF_1_3=(5*256)/3;
const unsigned char VREF_2_3=(2*(5*256))/3;

void main(void) {

	INTCON=0b00000000;
	OSCCON=0b01110000;		// 32MHz

	TRISA=0b00011110;
	LATA= 0b00000000;
	WPUA= 0b00001100;		// RESET,Threshold
	DACCON0=0b00000000;
	CM1CON0=0b00000000;
	CM1CON1=0b00000000;

	ANSELA=0b00010110;		// 3 analog
	init_a2d();


	while(1) {
		adc_pin2=read_a2d(3);		// diventa pin 3...
		adc_pin6=read_a2d(1);		// pin 6
		vref_pin5=read_a2d(2);		// pin 5
		if(vref_pin5==255)				// se non connesso, uso 2/3!
			vref_pin5=VREF_2_3;
	
		if(RA3) {		// RESET
			if(adc_pin6>vref_pin5) {
				OUT_PIN=0;
				DISCHARGE_PIN=1;
				}
			else {
				DISCHARGE_PIN=0;
				if(adc_pin2<VREF_1_3)
					OUT_PIN=1;
				}
			}
		else {
			OUT_PIN=1;
			DISCHARGE_PIN=0;
			}
//		__delay_us(10);
		}

	}
	
	
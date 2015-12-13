/* vim: set sw=8 ts=8 si : */
/*********************************************
* Author: Guido Socher, Copyright: GPL 
* 
* Digital to analog converter using a R-2R leadder
**********************************************/
#include <avr/io.h>
#include "avr_compat.h"

// dac takes an int instead of unsigned int to avoid conversion
// from the datatypes in analog.c but you can only use the positive values. 
void dac(int value){
	PORTD=value & 0xFF; // mask out 8 bits
	// 0x300=1100000000
	value=(value & 0x300)>>6; // mask out 2 msb bits and shift by 6
	// F3=11110011
	PORTC=(PORTC & 0xF3)|value; // change PC2 and PC3 only
}

void init_dac(void) 
{
	// enable PD0-PD7 as output PD0 is LSB in dac
	DDRD=0xff;
	// zero output
	PORTD=0;
	// enable PC3 as output (MSB in dac)
	sbi(DDRC,PC3);
	// zero output
	cbi(PORTC,PC3);
	// enable PC2 as output (MSB in dac)
	sbi(DDRC,PC2);
	// zero output
	cbi(PORTC,PC2);
}



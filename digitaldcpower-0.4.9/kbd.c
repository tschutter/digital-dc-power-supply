/* vim: set sw=8 ts=8 si : */
/*********************************************
* Author: Guido Socher, Copyright: GPL 
* 
* read the keyboard
**********************************************/
#include <avr/io.h>
#include "avr_compat.h"
#include "lcd.h"


// it looks like output port settings need time to propagate. Maybe
// caused by input capacitors on the lcd which connected there too.
static void kbd_wait(void){
	unsigned char i=100;
	while(i){i--;}
}

void init_kbd(void) 
{
	/* init lcd sets those already so we comment them out here:
	sbi(DDRB,PB7); // output line
	sbi(DDRB,PB2); // output line
	sbi(DDRB,PB1); // output line
	sbi(DDRB,PB0); // output line
	*/

	cbi(DDRB,PINB5); // input line
	cbi(DDRB,PINB3); // input line

	sbi(PORTB,PINB5); // internal pullup resistor on
	sbi(PORTB,PINB3); // internal pullup resistor on
}

unsigned char check_u_button(int *u) 
{
	// check U+ button:
	sbi(PORTB,PB7); 
	sbi(PORTB,PB2);
	sbi(PORTB,PB1);
	cbi(PORTB,PB0);
	kbd_wait();
	if (bit_is_clear(PINB,PINB5)){
		(*u)++;
		return(1);
	}
	// check U- button:
	sbi(PORTB,PB7); 
	sbi(PORTB,PB2);
	cbi(PORTB,PB1);
	sbi(PORTB,PB0);
	kbd_wait();
	if (bit_is_clear(PINB,PINB5) && (*u)>0){
		(*u)--;
		if((*u)<0){
			(*u)=0;
		}
		return(1);
	}
	return(0);
}

unsigned char check_i_button(int *i) 
{
	// check I+ button:
	sbi(PORTB,PB7); 
	cbi(PORTB,PB2);
	sbi(PORTB,PB1);
	sbi(PORTB,PB0);
	kbd_wait();
	if (bit_is_clear(PINB,PINB5)){
		(*i)++;
		return(1);
	}
	// check I- button:
	cbi(PORTB,PB7); 
	sbi(PORTB,PB2);
	sbi(PORTB,PB1);
	sbi(PORTB,PB0);
	kbd_wait();
	if (bit_is_clear(PINB,PINB5) && (*i)>0){
		(*i)--;
		if((*i)<0){
			(*i)=0;
		}
		return(1);
	}
	return(0);
}

unsigned char check_store_button(void) 
{
	// check store button:
	cbi(PORTB,PB2);
	kbd_wait();
	if (bit_is_clear(PINB,PINB3)){
		return(1);
	}
	return(0);
}



/* vim: set sw=8 ts=8 si : */
/*********************************************
* Author: Guido Socher, Copyright: GPL 
*
* Digital analog conversion of channel ADC0 and ADC1 in
* free running mode. 
**********************************************/
#include <avr/interrupt.h>
#include <avr/signal.h>
#include <avr/io.h>
#include <inttypes.h>
#include "avr_compat.h"
#include "dac.h"
#include "hardware_settings.h"


volatile static unsigned char channel=0; 
volatile static unsigned char voltagecontrol=0; // 0=current limit, 1=voltage control
volatile static uint16_t conversioncount=0; // changed at every conversion
// adc measurement results:
volatile static int analog_result[2];  // datatype int is 16 bit

// the value that is requested (control loop calibrates to this).
volatile static unsigned int target_val[2];  // datatype int is 16 bit

static unsigned char adlow,adhigh;
static int dac_val=0;

/* You must enable interrupt with sei() in the main program 
 * before you call init_analog 
 */
void init_analog(void) 
{
	// initialize the adc result to very high values
	// to keep the control-loop down until proper measurements
	// are done:
	analog_result[0]=0; 
	analog_result[1]=1025; 
	target_val[0]=0; // initialize to zero
	target_val[1]=0; // initialize to zero
        /* enable analog to digital conversion in free run mode
        *  without noise canceler function. See datasheet of atmega8 page 195
        * We set ADPS2=1,ADPS1=0,ADPS0=1 to have a clock division factor of 32.
        * This is needed to stay in the recommended range of 50-200kHz 
        * ADEN: Analog Digital Converter Enable
        * ADIE: ADC Interrupt Enable
        * ADIF: ADC Interrupt Flag
        * ADFR: ADC Free Running Mode
        * ADCSR: ADC Control and Status Register
        * ADPS2..ADPS0: ADC Prescaler Select Bits
	* REFS: Reference Selection Bits (page 203)
        */

	// int-ref with external capacitor at AREF pin: 
	// 2.56V int ref=REFS1=1,REFS0=1
	// write only the lower 3 bit for channel selection
	
	// 2.56V ref, start with channel 0
	ADMUX=(1<<REFS1)|(1<<REFS0)|(channel & 0x07);

        ADCSR=(1<<ADEN)|(1<<ADIE)|(1<<ADFR)|(1<<ADIF)|(1<<ADPS2)|(1<<ADPS0);

	/*  start conversion */
	sbi(ADCSR,ADSC);
	//wait until both channels are measured twice
	while(conversioncount < 4);
}

int is_dacval(void) 
{
	return(dac_val);
}

signed char is_current_limit(void) 
{
	// return 1 if current control loop active
	if (voltagecontrol==0){
		return(1);
	}
	return(0);
}

/* set the target adc value for the control loop
 * values for item: 0 = u, 1 = i, units must be of the same as the values 
 * from the dac.
 */
void set_target_adc_val(unsigned char item,unsigned int val) 
{
	// here we can directly write to target_val 
	target_val[item]=val;
}
/* get the result of an analog conversion.
 * The problem here is that we need to read more than 8 bit. Therefore
 * it will always result in two assembler instruction. In other words
 * it is not an atomic operation. To get it atomic we use the 
 * conversion count variable. If it was unchanged then the reading was
 * like an atomic read.
 */
int getanalogresult(unsigned char channel) 
{
	int rval;
	uint16_t conv_cnt_before;
	while(1){
		conv_cnt_before=conversioncount;
		rval=analog_result[channel];
		// make sure there was no AD conversion
		// while we were reading the result. If so then
		// read again. conversioncount is volatile.
		if (conversioncount == conv_cnt_before){
			break;
		}
	}
	return(rval);
}

// the control loop changes the dac:
static void control_loop(unsigned char channel){
	int tmp;
	tmp=target_val[0] - analog_result[0];
	if (tmp < 1){
		voltagecontrol=0; // I control
	}else{
		voltagecontrol=1; // U control
		tmp=target_val[1] - analog_result[1];
	}
	if (conversioncount>2){ 
		// go only once in a while close
		// to prevent permanent lsb toggle
		if (tmp > -2 && tmp < 2){
			tmp=0;
		}
	}
	// slowly up:
	if (tmp >1){
		tmp=1;
	}
	if (tmp <-1){
		tmp=-1;
	}
	dac_val=dac_val + tmp;
	if (dac_val<0){
		dac_val=0;
	}
	dac(dac_val);
}
/* the following function will be called when analog conversion is done.
 * It will be called every 13th cycle of the converson clock. At 4Mhz
 * and a ADPS factor of 32 this is: ((4000 kHz/ 32) / 13)= 9.6KHz intervalls
 */
SIGNAL(SIG_ADC) {
	adlow=inp(ADCL); /* read low first !! */
	adhigh=inp(ADCH); /* read low first !! */
	// toggel the channel between 0 an 1. This will however
	// not effect the next conversion as that one is already
	// ongoing.
	channel++;
	if (channel>1){
		channel=0;
	}
	// 2.56V ref
	ADMUX=(1<<REFS1)|(1<<REFS0)|(channel & 0x07);
	// channel=1 = U, channel=0 = I
	analog_result[channel]=((adhigh<<8)|(adlow & 0xFF));

	// short circuit protection:
	if (analog_result[0] > SH_CIR_PROT){
		dac(20);
		dac_val=20;
		goto ENDSIG;
	}
	if (channel == 1){
		// only when we have new current measurements
		goto ENDSIG;
	}
	control_loop(voltagecontrol);
	// end of interrupt handler
ENDSIG:
	conversioncount++; 
}


/*********************************************
* vim: set sw=8 ts=8 si :
* Author: Guido Socher, Copyright: GPL 
* This is the main program for the digital dc power supply
* 
* See http://www.tuxgraphics.org/electronics/
* 
* Chip type           : ATMEGA8
* Clock frequency     : Internal clock 4 Mhz 
*********************************************/
#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#define F_CPU 4000000UL  // 4 MHz
#include <avr/delay.h>
#include "lcd.h"
#include "dac.h"
#include "kbd.h"
#include "analog.h"
#include "avr_compat.h"
#include "hardware_settings.h"
#include "i2c_avr.h"
#include <stdlib.h> // atoi
#include <string.h> 
#include <avr/eeprom.h> 

#define SWVERSION "ver: ddcp-0.4.8"

static int set_val[2];
static int measured_val[2];
static char i2c_buf[MAX_BUF_LEN+1];
// the set values but converted to ADC steps
static int set_val_adcUnits[2]; 
static unsigned char bpress=0;

void delay_ms(unsigned int ms)
/* delay for a minimum of <ms> */
{
        // we use a calibrated macro. This is more
        // accurate and not so much compiler dependent
        // as self made code.
        while(ms){
                _delay_ms(0.96);
                ms--;
        }
}

// Convert a integer which is representing a float into a string.
// decimalpoint_pos sets the decimal point after 2 pos: e.g 74 becomes "0.74"
// The integer may not be larger than 10000.
// The integer must be a positive number.
// spacepadd can be used to add a leading speace if number is less than 10
static void int_to_ascii(int inum,char *outbuf,signed char decimalpoint_pos,signed char spacepadd){
	signed char i,j;
	char chbuf[8];
	j=0;
	while(inum>9 && j<7){
		// zero is ascii 48:
		chbuf[j]=(char)48+ inum-((inum/10)*10);
		inum=inum/10;
		j++;
		if(decimalpoint_pos==j){
			chbuf[j]='.';
			j++;
		}
	}
	chbuf[j]=(char)48+inum; // most significant digit
	decimalpoint_pos--;
	while(j<decimalpoint_pos){
		j++;
		chbuf[j]='0';
	}
	if (spacepadd && j > (decimalpoint_pos+2)){
		// no leading space padding needed
		spacepadd=0;
	}
	if(decimalpoint_pos==j){
		j++;
		chbuf[j]='.';
		j++;
		chbuf[j]='0'; // leading zero
	}
	if (spacepadd){
		j++;
		chbuf[j]=' '; // leading space padding: "9.50" becomes " 9.50"
	}
	// now reverse the order 
	i=0;
	while(j>=0){
		outbuf[i]=chbuf[j];
		j--;
		i++;
	}
	outbuf[i]='\0';
}

// convert voltage values to adc values, disp=10 is 1.0V
static int disp_u_to_adc(int disp){
	return((int)(disp * 102.3) / (ADC_REF * U_DIVIDER));
}
// calculate the needed adc offset for voltage drop on the
// current measurement shunt (the shunt has about 0.5 Ohm =1/2 Ohm)
static int disp_i_to_u_adc_offset(int disp){
	return(disp_u_to_adc(disp/20));
}
// convert adc values to voltage values, disp=10 is 1.0V
// disp_i_val is needed to calculate the offset for the voltage drop over
// the current measurement shunt
static int adc_u_to_disp(int adcunits,int disp_i_val){
	int adcdrop;
	adcdrop=disp_i_to_u_adc_offset(disp_i_val);
	if (adcunits < adcdrop){
		return(0);
	}
	adcunits=adcunits-adcdrop;
	return((int)(((adcunits /102.3)* ADC_REF * U_DIVIDER)+0.6));
}
// convert adc values to current values, disp=10 needed to be printed
// by the printing function as 0.10 A
static int disp_i_to_adc(int disp){
	return((int) (((disp * 10.23)* I_RESISTOR) / ADC_REF));
}
// convert adc values to current values, disp=10 needed to be printed
// by the printing function as 0.10 A
static int adc_i_to_disp(int adcunits){
	return((int) (((adcunits* ADC_REF)/(10.23 * I_RESISTOR))+0.6));
}

static void store_permanent(void){
	int tmp;
	signed char changeflag=1;
	lcd_clrscr();
	if (eeprom_read_byte((uint8_t *)0x0) == 19){
		changeflag=0;
		// ok magic number matches accept values
		tmp=eeprom_read_word((uint16_t *)0x04);
		if (tmp != set_val[1]){
			changeflag=1;
		}
		tmp=eeprom_read_word((uint16_t *)0x02);
		if (tmp != set_val[0]){
			changeflag=1;
		}
	}
	if (changeflag){
		lcd_puts_P("setting stored");
		eeprom_write_byte((uint8_t *)0x0,19); // magic number
		eeprom_write_word((uint16_t *)0x02,set_val[0]);
		eeprom_write_word((uint16_t *)0x04,set_val[1]);
	}else{
		if (bpress> 5){
			// display software version after long press
			lcd_puts_P(SWVERSION);
			lcd_gotoxy(0,1);
			lcd_puts_P("tuxgraphics.org");
		}else{
			lcd_puts_P("already stored");
		}
	}
	delay_ms(200);
}

// check the keyboard
static unsigned char check_buttons(void){
	if (check_u_button(&(set_val[1]))){
		if(set_val[1]>U_MAX){
			set_val[1]=U_MAX;
		}
		return(1);
	}
	if (check_i_button(&(set_val[0]))){
		if(set_val[0]>I_MAX){
			set_val[0]=I_MAX;
		}
		return(1);
	}
	if (check_store_button()){
		store_permanent();
		return(2);
	};
	return(0);
}

void check_i2c_interface(void){
	if (i2c_get_received_data(i2c_buf)){
		if (i2c_buf[0]=='i'){
			if (i2c_buf[1]=='=' && i2c_buf[2]!='\0'){
				set_val[0]=atoi(&i2c_buf[2]);
				if(set_val[0]>I_MAX){
					set_val[0]=I_MAX;
				}
				if(set_val[0]<0){
					set_val[0]=0;
				}
				i2c_send_data("ok");
			}else{
				int_to_ascii(measured_val[0],i2c_buf,2,0);
				strcat(i2c_buf,"A");
				i2c_send_data(i2c_buf);
			}
		}else if (i2c_buf[0]=='s'){
			store_permanent();
			i2c_send_data("ok");
		}else if (i2c_buf[0]=='u'){
			if (i2c_buf[1]=='=' && i2c_buf[2]!='\0'){
				set_val[1]=atoi(&i2c_buf[2]);
				if(set_val[1]>U_MAX){
					set_val[1]=U_MAX;
				}
				if(set_val[1]<0){
					set_val[1]=0;
				}
				i2c_send_data("ok");
			}else{
				int_to_ascii(measured_val[1],i2c_buf,1,0);
				strcat(i2c_buf,"V");
				i2c_send_data(i2c_buf);
			}
		}else{
			i2c_send_data("err");
		}
	}
}

int main(void)
{
	char out_buf[20+1];
	measured_val[0]=0;
	measured_val[1]=0;
	init_dac();
	lcd_init(LCD_DISP_ON);
	init_kbd();
	set_val[0]=15;set_val[1]=50; // 150mA and 5V
	if (eeprom_read_byte((uint8_t *)0x0) == 19){
		// ok magic number matches accept values
		set_val[1]=eeprom_read_word((uint16_t *)0x04);
		set_val[0]=eeprom_read_word((uint16_t *)0x02);
	}
	// I2C also called TWI
	i2c_init(3,1,0);
	sei();
	i2c_send_data("on");
	init_analog();
	while (1) {

		// current
		measured_val[0]=adc_i_to_disp(getanalogresult(0));
		set_val_adcUnits[0]=disp_i_to_adc(set_val[0]);
		set_target_adc_val(0,set_val_adcUnits[0]);
		// voltage
		measured_val[1]=adc_u_to_disp(getanalogresult(1),measured_val[0]);
		set_val_adcUnits[1]=disp_u_to_adc(set_val[1])+disp_i_to_u_adc_offset(measured_val[0]);
		set_target_adc_val(1,set_val_adcUnits[1]);

		// voltage
		lcd_clrscr();
		int_to_ascii(measured_val[1],out_buf,1,1);
		lcd_puts(out_buf);
		lcd_puts("V ");
		int_to_ascii(set_val[1],out_buf,1,1);
		lcd_putc('[');
		lcd_puts(out_buf);
		lcd_putc(']');
		if (!is_current_limit()){
			// put a marker to show which value is currenlty limiting
			lcd_puts("<-");
		}

		// current
		lcd_gotoxy(0,1);
		int_to_ascii(measured_val[0],out_buf,2,0);
		lcd_puts(out_buf);
		lcd_puts("A ");
		int_to_ascii(set_val[0],out_buf,2,0);
		lcd_putc('[');
		lcd_puts(out_buf);
		lcd_putc(']');
		if (is_current_limit()){
			// put a marker to show which value is currenlty limiting
			lcd_puts("<-");
		}
		//dbg
		//int_to_ascii(is_dacval(),out_buf,0,0);
		//lcd_puts(out_buf);
		check_i2c_interface();

		// the buttons must be responsive but they must not 
		// scroll too fast if pressed permanently
		if (check_buttons()==0){
			// no buttons pressed
			delay_ms(100);
			bpress=0;
			check_i2c_interface();
			check_buttons();
			delay_ms(150);
		}else{
			// button press
			if (bpress > 11){
				// somebody pressed permanetly the button=>scroll fast
				delay_ms(10);
				check_i2c_interface();
				delay_ms(40);
			}else{
				bpress++;
				delay_ms(100);
				check_i2c_interface();
				delay_ms(150);
			}
		}
	}
	return(0);
}


/*
 * vim: set sw=8 ts=8 si :
 * Target: Atmega8
 * Title: I2C interface based on the AVR Two-Wire Interface (TWI) hardware
 * Description : I2C ("eye-squared-see") is a two-wire bidirectional 
 *               communication interface. It was invented by Philips and
 *               they have protected this name. This is why other manufactures
 *               use a different name for the same protocol.
 *
 *               The two wires are called SDA and SCL. The SCL is the clock
 *               line which is controlled by the master on the i2c bus.
 *
 *  		 SCL is pin PC5 and SDA is pin PC4 for an Atmega8
 */

#include <avr/io.h>
#include <avr/signal.h>
#include <avr/interrupt.h>
#include <string.h>
#include "i2c_avr.h"
#include "avr_compat.h"

#define TWSR_STATUS_MASK	0xF8
#define TWCR_CMD_MASK		0x0F

/* TWI states, slave transmitter */
#define TW_ST_SLA_ACK           0xA8
#define TW_ST_ARB_LOST_SLA_ACK  0xB0
#define TW_ST_DATA_ACK          0xB8
#define TW_ST_DATA_NACK         0xC0
#define TW_ST_LAST_DATA         0xC8
/* TWI states, slave receiver */
#define TW_SR_SLA_ACK           0x60
#define TW_SR_ARB_LOST_SLA_ACK  0x68
#define TW_SR_GCALL_ACK         0x70
#define TW_SR_ARB_LOST_GCALL_ACK 0x78
#define TW_SR_DATA_ACK          0x80
#define TW_SR_DATA_NACK         0x88
#define TW_SR_GCALL_DATA_ACK    0x90
#define TW_SR_GCALL_DATA_NACK   0x98
#define TW_SR_STOP              0xA0
/* Misc */
#define TW_NO_INFO              0xF8
#define TW_BUS_ERROR            0x00


volatile static uint8_t slave_state=0; // 1 = receive, 2 = transmit, 0 = ready (all data recieved or sent
// slave receiver buffer (we expect \0 terminated strings in there):
volatile static uint8_t slave_rec_buf[MAX_BUF_LEN+1];
volatile static uint8_t slave_rec_buf_pos=0; 
// slave_tx_buf is a \0 terminated buffer
volatile static uint8_t slave_tx_buf[MAX_BUF_LEN+1];
volatile static uint8_t slave_tx_buf_pos=0; 


void i2c_init(uint8_t devadr, uint8_t gencall,uint8_t pullup)
{
	uint8_t i;
	// init the TWI as a slave receiver or transmitter. Transmit
	// or receive depends on the direction bit received from the master
	// (the bit after the address). Zero in the direction bit means write
	// = transmit from master = we receive.
	//
	// enable TWI (two wire interface)
	// You will have to call sei() in the main program to use this.
	if (pullup){
		// use internal pull-up resistor instead of external 4.7k
		sbi(PORTC,PINC4);
	}
	slave_state=0;
	slave_rec_buf_pos=0;
	slave_tx_buf_pos=0;
	for(i=0;i<=MAX_BUF_LEN;i++){
		slave_tx_buf[i]='\0';
	}
        // enable TWI interrupt and slave address ACK
	outp((BV(TWINT)|BV(TWEA)|BV(TWEN)|BV(TWIE)),TWCR);
	// set local device address for the slave, must be done after setting of TWCR
	devadr=devadr<<1;
	outp(((devadr & 0xFE) | (gencall ?1:0)),TWAR );
	// The Address Match unit checks if received address bytes match 
	// the seven-bit address in the TWI Address Register (TWAR). If 
	// the TWI General Call Recognition Enable (TWGCE) bit in the TWAR 
	// is written to one. If the TWGCE is set, the TWI will respond to 
	// the general call address (0x00).

        // When the address is received from the master it is followed by
        // a 0 to indicate that the slave should receive or 1 to indicate
        // that it should transmit.

}

// get the data received from master.
// It has to be a null char terminated string
// It reads whatever is in slave_rec_buf
uint8_t i2c_get_received_data(char *i2c_outbuf)
{
	if (slave_state !=1 && slave_rec_buf_pos >0 && slave_rec_buf[slave_rec_buf_pos-1]=='\0'){
		slave_rec_buf[MAX_BUF_LEN]='\0';
		strncpy(i2c_outbuf,(char *) slave_rec_buf,MAX_BUF_LEN);
		// clear buffer
		slave_rec_buf_pos=0;
		return(1);
	}else{
		// no data available
		return(0);
	}
}

// send a zero terminated data string. The send will be done
// when the master wants us to do so. I.e it may not be send at all.
// you have to have a high level handshake protocol with the master
// if you want to be sure that all data was sent.
uint8_t i2c_send_data(char *i2c_data)
{
	while(slave_state==2); // we are still transmitting data. wait here.
	strncpy((char *) slave_tx_buf, i2c_data, MAX_BUF_LEN);
	slave_tx_buf[MAX_BUF_LEN]='\0';
	slave_tx_buf_pos=0;
	return(0);
}


// I2C (TWI) interrupt service routine
SIGNAL(SIG_2WIRE_SERIAL)
{
        // read status bits
        uint8_t ack=0; // 0: send no ack, 1:send TWEA=1=ack,2 recover from bus error 
        uint8_t status = inp(TWSR) & TWSR_STATUS_MASK;
        switch(status)
        {

	// ---- Slave Receiver
	//
	// 0x60: own SLA+W has been received, ACK has been returned
	case TW_SR_SLA_ACK: 
	// 0x68: own SLA+W has been received, ACK has been returned
	case TW_SR_ARB_LOST_SLA_ACK:
	// 0x70: General call address has been received, ACK has been returned
	case TW_SR_GCALL_ACK:	
	// 0x78:     GCA+W has been received, ACK has been returned
	case TW_SR_ARB_LOST_GCALL_ACK:		
		// we are being addressed as slave for 
		// writing (data will be received from master)
		// set state
		slave_state = 1; 
		// prepare buffer
		slave_rec_buf_pos = 0;
		// receive data byte and return ACK
		ack=1;
		break;
	// 0x80: data byte has been received, ACK has been returned
	case TW_SR_DATA_ACK:		
	// 0x90: data byte has been received, ACK has been returned
	case TW_SR_GCALL_DATA_ACK:	
		// get received data byte
		if (slave_rec_buf_pos>= MAX_BUF_LEN){
			// too much data
			slave_rec_buf[MAX_BUF_LEN]='\0';
			slave_rec_buf_pos=MAX_BUF_LEN+1;
			//slave_state = 0; // expect no more data
			// return NACK
		}else{
			slave_rec_buf[slave_rec_buf_pos] = inp(TWDR);
			slave_rec_buf_pos++;
			// return ACK
			ack=1;
		}
		break;
	// 0x88: data byte has been received, NACK has been returned
	case TW_SR_DATA_NACK:		
	// 0x98: data byte has been received, NACK has been returned
	case TW_SR_GCALL_DATA_NACK:	
		// receive data byte and return NACK
		// get received data byte
		if (slave_rec_buf_pos>= MAX_BUF_LEN){
			// too much data
			slave_rec_buf[MAX_BUF_LEN]='\0';
			slave_rec_buf_pos=MAX_BUF_LEN+1;
		}else{
			slave_rec_buf[slave_rec_buf_pos] = inp(TWDR);
			slave_rec_buf_pos++;
		}
		// return NACK
		//slave_state = 0; // expect no more data
		break;
	// 0xA0: STOP or REPEATED START has been received while addressed as slave
	case TW_SR_STOP:
		// switch to SR mode with SLA ACK
		ack=1;
		// i2c receive is complete
		// set state
		slave_state = 0;
		break;

	// ---- Slave Transmitter
	//
	// 0xA8: own SLA+R has been received, ACK has been returned
	case TW_ST_SLA_ACK:
	// 0xB0: GCA+R has been received, ACK has been returned
	case TW_ST_ARB_LOST_SLA_ACK:
		// we are being addressed as slave for 
		// reading (data must be transmitted back to master)
		// set state
		slave_state = 2;
		slave_tx_buf_pos=0;
		// reset data index
		// fall-through to transmit first data byte
	// 0xB8: data byte has been transmitted, ACK has been received
	case TW_ST_DATA_ACK:				
		// the master has to know how many bytes it should read
		// the last byte must get a nack from the master.
		if (slave_tx_buf_pos < MAX_BUF_LEN && slave_tx_buf[slave_tx_buf_pos]){
			// we have data, transmit data byte
			outp(slave_tx_buf[slave_tx_buf_pos],TWDR);
			slave_tx_buf_pos++;
			// expect ACK to data byte
			ack=1;
		}else{
			outp(0,TWDR);
			// expect NACK to data byte
			slave_tx_buf_pos=0;
			slave_state = 0;
		}
		break;
	// 0xC0: data byte has been transmitted, NACK has been received
	case TW_ST_DATA_NACK:
	// 0xC8: Last data byte in TWDR has been transmitted (TWEA =  0 ); ACK has been received
	case TW_ST_LAST_DATA:
		// all done
		// switch to open slave
		ack=1;
		slave_tx_buf_pos=0;
		// set state
		slave_state = 0;
		break;

	// ---- Misc

	// Status 0xF8 indicates that no relevant information is available
	// because the TWINT Flag is not set. This occurs between other
	// states, and when the TWI is not involved in a serial transfer.
	case TW_NO_INFO:
		// do nothing
		break;
	// Status 0x00 indicates that a bus error has occurred during a
	// Two-wire Serial Bus trans-fer. A bus error occurs when a START
	// or STOP condition occurs at an illegal position in the format
	// frame. Examples of such illegal positions are during the serial
	// transfer of an address byte, a data byte, or an acknowledge
	// bit. When a bus error occurs, TWINT is set. To recover from a
	// bus error, the TWSTO Flag must set and TWINT must be cleared
	// by writing a logic one to it. This causes the TWI to enter the
	// not addressed Slave mode and to clear the TWSTO Flag (no other
	// bits in TWCR are affected). The SDA and SCL lines are released,
	// and no STOP condition is transmitted.
	case TW_BUS_ERROR:
		// reset internal hardware by falling to the default case
	default:
		// we should actually only come here if we forgott
		// something in the above list. That should normally
		// not be the case. Still we just act 
		//
		// set state
		slave_rec_buf_pos=0;
		slave_tx_buf_pos=0;
		slave_state = 0;
		// reset internal hardware 
		ack=2;
		break;
	}
	// clear the interrupt flag and give control to start again
	if (ack==2){
		// reset
		outp((BV(TWINT)|BV(TWEA)|BV(TWSTO)|BV(TWEN)|BV(TWIE)),TWCR);
	}
	if (ack==1){
		// normal ack
		outp((BV(TWINT)|BV(TWEA)|BV(TWEN)|BV(TWIE)),TWCR);
	}
	if (ack==0){
		// normal nack
		outp((BV(TWINT)|BV(TWEN)|BV(TWIE)),TWCR);
	}
}

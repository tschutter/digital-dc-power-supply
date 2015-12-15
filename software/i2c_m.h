/* vim: set sw=8 ts=8 si : */
/* header for i2c master over rs232 on linux. Written
 * by guido socher. License = GPL */
#ifndef I2C_M_H
#define I2C_M_H

// copied form i2c_avr.h:
#define MAX_BUF_LEN 31
// the functions which you will normally use are:
int open_tty(char *comport);
int address_slave(unsigned char devadr,int read_data);
int i2cstop(void);
// receive a string to received_char_string. received_char_string must
// be at least MAX_BUF_LEN+1 long
void i2c_rx_string(char *received_char_string);
// sent a string no longer than MAX_BUF_LEN
int i2c_tx_string(char *stringval);
int close_tty(void);

// you have also access to the following functions
int i2cstart(void);
void i2c_tx_bit(int val);
void scl_high(void);
void scl_low(void);
void sda_high(void);
void sda_low(void);
int wait_port(void);
int sda_value(void);
unsigned int i2c_rx_bit(void);
int i2c_rx_ack();
void i2c_tx_ack(int ack);
int i2c_tx_byte(unsigned char byteval);
// if gen_ack =1 always generate ack, gen_ack =2 only generate ack if byte != '\0'
unsigned char i2c_rx_byte(int gen_ack);

#endif 

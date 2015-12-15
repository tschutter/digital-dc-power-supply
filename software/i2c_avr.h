/* vim: set sw=8 ts=8 si : */
/* header for i2c slave on ATMEGA8. Written
 * by guido socher. License = GPL */
#ifndef I2C_AVR_H
#define I2C_AVR_H

#define MAX_BUF_LEN 31

// the functions which you will normally use are:
void i2c_init(uint8_t devadr, uint8_t gencall,uint8_t pullup);
uint8_t i2c_get_received_data(char *i2c_outbuf);
uint8_t i2c_send_data(char *i2c_data);

#endif 

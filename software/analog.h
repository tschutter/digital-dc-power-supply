/* vim: set sw=8 ts=8 si : */
/*************************************************************************
 Title	:   C include file for analog conversion
 Target:    any AVR device
 Copyright: GPL
***************************************************************************/
#ifndef ANALOG_H
#define ANALOG_H

/* You must enable interrupt with sei() in the main program */
extern void init_analog(void);
/* get the result of an analog conversion */
extern int getanalogresult(unsigned char channel);
extern void set_target_adc_val(unsigned char item,int val);
extern signed char is_current_limit(void); 
extern int is_dacval(void);

#endif /* ANALOG_H */

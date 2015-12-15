/* vim: set sw=8 ts=8 si : */
/*************************************************************************
 Title	:   C include file for digital to analog conversion
 Copyright: GPL
***************************************************************************/
#ifndef DAC_H
#define DAC_H

extern void init_dac(void);
// set a certain value:
extern void dac(int value);

#endif 

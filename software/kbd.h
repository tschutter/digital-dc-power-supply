/* vim: set sw=8 ts=8 si : */
/*************************************************************************
 Title	:   include file for the keyboard of the digital power supply
 Copyright: GPL
***************************************************************************/
#ifndef KBD_H
#define KBD_H

extern void init_kbd(void);
extern unsigned char check_u_button(int *u);
extern unsigned char check_i_button(int *i);
extern unsigned char check_store_button(void);

#endif 

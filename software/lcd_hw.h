/*************************************************************************
 Title	:   C include file for the HW defs of for the HD44780U LCD library
 Author:    Markus Ermert 
***************************************************************************/
#ifndef LCD_HW_H
#define LCD_HW_H


/* change these definitions according to your hardware */


#define LCD_DATA_PIN_D7  PB7	/* arbitrary pin for LCD data line D7 */
#define LCD_DATA_PIN_D6  PB2	/* arbitrary pin for LCD data line D7 */
#define LCD_DATA_PIN_D5  PB1	/* arbitrary pin for LCD data line D5 */
#define LCD_DATA_PIN_D4  PB0	/* arbitrary pin for LCD data line D4 */

#define LCD_DATA_DDR_D7 DDRB	/* ddr for LCD data line D7 */
#define LCD_DATA_DDR_D6 DDRB	/* ddr for LCD data line D6 */
#define LCD_DATA_DDR_D5 DDRB	/* ddr for LCD data line D5 */
#define LCD_DATA_DDR_D4 DDRB	/* ddr for LCD data line D4 */

#define LCD_DATA_PORT_D7 PORTB	/* port for LCD data line D7 */
#define LCD_DATA_PORT_D6 PORTB	/* port for LCD data line D6 */
#define LCD_DATA_PORT_D5 PORTB	/* port for LCD data line D5 */
#define LCD_DATA_PORT_D4 PORTB	/* port for LCD data line D4 */

#define LCD_RS_DDR       DDRB   /* ddr for RS line */
#define LCD_RS_PORT      PORTB  /* port for RS line */
#define LCD_RS_PIN       PB6
#define LCD_E_DDR        DDRB  /* ddr for Enable line */
#define LCD_E_PORT       PORTB  /* port for Enable line */
#define LCD_E_PIN        PB4


/* normally you do not change the following */
#define LCD_LINES           2     /* visible lines */
#define LCD_LINE_LENGTH  0x40     /* internal line length */

/* no need to change the next 4 lines. This is standard for HD44780 */
#define LCD_START_LINE1  0x00     /* DDRAM address of first char of line 1 */
#define LCD_START_LINE2  0x40     /* DDRAM address of first char of line 2 */
#define LCD_START_LINE3  0x14     /* DDRAM address of first char of line 3 */
#define LCD_START_LINE4  0x54     /* DDRAM address of first char of line 4 */

#endif //LC_HW_H


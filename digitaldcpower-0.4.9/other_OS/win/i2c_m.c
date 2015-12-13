/* 
 * vim: set sw=8 ts=8 si et :
 * i2c code to implement a i2c connection via the rc232 interface
 * Some of the comments in this code are taken from the i2c faq by Vincent Himpe
 */

/* hardware connections: SDA is DTR, SCL is RTS, SDA input is CD 
 * On a 9 pin sub-d connector this is:
 * RTS= pin 7, DTR = pin 4, CD = 1, GND = 5
 *
 * Written by Guido Socher <guido at linuxfocus.org>
 * Copyright: GPL
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>

#include "i2c_m.h"

#define DBG 0


static HANDLE fd;

void scl_high(void){
        BOOL ok;
        ok=EscapeCommFunction(fd, SETRTS);
        if (!ok){
                fprintf(stderr, "File %s Line %d Function EscapeCommFunction failed (%.8x)\n",
                       __FILE__, __LINE__, (unsigned int)GetLastError());
                exit(1);
        }
        if (DBG) printf("c");/* dbg */
}

void scl_low(void){
        BOOL ok;
        ok=EscapeCommFunction(fd, CLRRTS);
        if (!ok){
                fprintf(stderr, "File %s Line %d Function EscapeCommFunction failed (%.8x)\n",
                       __FILE__, __LINE__,(unsigned int) GetLastError());
                exit(1);
        }
        if (DBG) printf("_");/* dbg */
}

void sda_high(void){
        BOOL ok;
        ok=EscapeCommFunction(fd, SETDTR);
        if (!ok){
                fprintf(stderr, "File %s Line %d Function EscapeCommFunction failed (%.8x)\n",
                       __FILE__, __LINE__,(unsigned int) GetLastError());
                exit(1);
        }
        if (DBG) printf("1");/* dbg */
}

void sda_low(void){
        BOOL ok;
        ok=EscapeCommFunction(fd, CLRDTR);
        if (!ok){
                fprintf(stderr, "File %s Line %d Function EscapeCommFunction failed (%.8x)\n",
                       __FILE__, __LINE__,(unsigned int) GetLastError());
                exit(1);
        }
        if (DBG) printf("0");/* dbg */
}
int wait_port(void){
        if (DBG) printf("w");/* dbg */
        Sleep(8); // unit is millisec
        return(0);
}

/* open port and set sda/scl to high */
int open_tty(char *comport){
        char CommPath[42];
        DCB Dcb;
        BOOL ok;

        snprintf(CommPath,40,"\\\\.\\%s",comport); // COM1 or COM2 or ...
	CommPath[40]='\0';
        fd = CreateFile(CommPath,
                      GENERIC_READ | GENERIC_WRITE,
                      0,
                      NULL,
                      OPEN_EXISTING,
                      0,
                      NULL);

        if (fd == INVALID_HANDLE_VALUE) {
            fprintf(stderr, "CreateFile failed: CommPath (%.8x)\n", (unsigned int)GetLastError());
            exit(1);
        }

        memset(&Dcb, 0x0, sizeof(DCB));
        Dcb.DCBlength = sizeof (DCB);
        ok = GetCommState(fd, &Dcb);
        if (!ok){
                fprintf(stderr, "File %s Line %d Function GetCommState failed (%.8x)\n",
                       __FILE__, __LINE__,(unsigned int) GetLastError());
                exit(1);
        }
 
 	// some reasonable defaults even if we do not use Tx/Rx:
 	Dcb.Parity=NOPARITY;
 	Dcb.BaudRate=9600;
 	Dcb.ByteSize=8;
 	Dcb.StopBits=ONESTOPBIT;

        Dcb.fDtrControl = DTR_CONTROL_ENABLE;
        Dcb.fRtsControl = RTS_CONTROL_ENABLE;
        ok = SetCommState(fd, &Dcb);
        if (!ok){
                fprintf(stderr, "File %s Line %d Function SetCommState failed (%.8x)\n",
                       __FILE__, __LINE__,(unsigned int) GetLastError());
                exit(1);
        }
 
        scl_high();
        return(0);
}

int close_tty(void){
        scl_low();
        wait_port();
        sda_low(); 
        CloseHandle(fd);
        return(0);

}

int i2cstart(void)
{
/* A start condition looks like this:

     SDA H ------\             The chip issuing the Start condition first pulls
         L        \-------     the SDA (data) line low. And next
                               pulls the SCL (clock) line low.
     SCL H ---------\
         L           \----
*/
        if (DBG) printf("S");//dbg
        sda_high();
        scl_high();
        wait_port();
        sda_low(); 
        wait_port();
        scl_low(); 
        if (DBG) printf("|");//dbg
        return(0);
}

int i2cstop(void)
{
        if (DBG) printf("P");/* dbg */
/* A stop condition looks like this:

     SDA H           /----     The Bus master first releases the SCL and then
         L ---------/          the SDA line

     SCL H        /-------
         L ------/
*/
        scl_low(); /* should already be the case */
        sda_low();
        scl_high();
        wait_port();
        sda_high(); 
        wait_port();
        if (DBG) printf("|\n");/* dbg */
        return(0);
}

/* val must be 1 or 0 */
void i2c_tx_bit(int val){
        scl_low();
        wait_port();
        if (val){
                sda_high();
        }else{
                sda_low();
        }
        scl_high();
        wait_port();
        scl_low();
}

/* read from SDA */
int sda_value(void){
        DWORD s;
        BOOL ok;
        ok = GetCommModemStatus(fd, &s);
        if (!ok){
                fprintf(stderr, "File %s Line %d Function GetCommModemStatus failed (%.8x)\n",
                       __FILE__, __LINE__,(unsigned int) GetLastError());
                exit(1);
        }
        if ( s & MS_RLSD_ON ){ // pin 1, CD
                return(1);
        }
        return(0);
}

unsigned int i2c_rx_bit(void){
        unsigned int val;
        scl_low();
        wait_port();
        scl_high();
        wait_port();
        val=sda_value();
        if (DBG){if(val) printf("."); else printf("o");} /* dbg */
        scl_low();
        return(val);
}

/* wait for an ack, done after addressing a chip */
int i2c_rx_ack(){
        /* we put sda high and then check (SCL=1) if
        slave has taken sda low. If yes then slave as given ack. */
        int ack_found=0;
        int i=0;
        sda_high(); /* release sda */
        wait_port();
        scl_high();
        while(i<20 && ack_found==0){
                i++;
                if (! sda_value()){
                        /* sda is low -> ack found */
                        ack_found=1;
                        wait_port();
                }else{
                        Sleep(70);
                }
        }
        scl_low();
        return(ack_found);
}

/* ack=1 send ack, ack=0 send nack */
void i2c_tx_ack(int ack){
        if (ack){
                i2c_tx_bit(0);
        }else{
                i2c_tx_bit(1);
        }
}

/* transmit a byte, return 1 if ack'ed by other side */
int i2c_tx_byte(unsigned char byteval){
        int i=0;
        /* msb is transmitted before lsb */
        for(i=7;i>=0;i--){
                if (byteval & (1<<i)){
                        i2c_tx_bit(1);
                }else{
                        i2c_tx_bit(0);
                }
        }
	i=i2c_rx_ack(); /* get ack */
        if (DBG) printf("\n"); /* dbg */
        return(i);
}

/* if gen_ack =1 always generate ack, gen_ack =2 only generate ack if byte != '\0' */
unsigned char i2c_rx_byte(int gen_ack){
        int i;
        unsigned char recdat=0;
        /* release sda */
        sda_high();
        /* msb is transmitted before lsb */
        for(i=7;i>=0;i--){
                if (i2c_rx_bit()){
                        recdat=recdat | (1<<i);
                }
        }
        if (gen_ack==2){
                if (recdat == '\0'){
                        gen_ack=0;
                }
        }
        i2c_tx_ack(gen_ack);
        if (DBG) printf("\n"); /* dbg */
        return (recdat);
}

/* sent a string no longer than MAX_BUF_LEN */
int i2c_tx_string(char *stringval){
        int i=0;
        int gotack=1;
        if (DBG) printf("tx_string:"); /* dbg */
	// 32 is ascii space and everything before space is non printable
        while(i<MAX_BUF_LEN && *(stringval+i) && (int)*(stringval+i) > 31){
                // we expect ack for all bytes:
                if (!i2c_tx_byte((unsigned char) *(stringval+i))){
                        gotack=0;
                }
                i++;
        }
        /* the last byte (zeor byte) may get a nack: */
        i2c_tx_byte(0);
	Sleep(450); /* unit is milliseconds, give the uC a chance to poll the data */
        return(gotack);
}

/* receive a string to received_char_string. received_char_string must
 be at least MAX_BUF_LEN+1 long */
void i2c_rx_string(char *received_char_string){
        int i=0;
        *received_char_string=1; // something not zero */
        /* the code in the uC (i2c_avr.c) sends a zero when there is
         no more data: */
        if (DBG) printf("rx_string:"); /* dbg */
        while(i<MAX_BUF_LEN){
                *(received_char_string+i)=i2c_rx_byte(2);
                if (*(received_char_string+i)=='\0'){
                        break;
                }
                i++;
        }
        /* in case we did not get a zero: */
        *(received_char_string+MAX_BUF_LEN)='\0';
}

/* When the address is received from the master it is followed by
 a 0 to indicate that the slave should receive or 1 to indicate
 that it should transmit.

 read_data = 1 -> slave should transmit, we read data from the
 slave.
 address_slave returns 1 if slave did ack (it is there) otherwise 0
 devadr=0 is the general call address */
int address_slave(unsigned char devadr,int read_data){
        /* this function will send: 
         | start | 7-bit slave adr | read_data bit | wait for ack */
        unsigned char byteval;
        i2cstart();
        byteval=((devadr << 1) & 0xFE)|(read_data ?1:0);
        return(i2c_tx_byte(byteval));
}




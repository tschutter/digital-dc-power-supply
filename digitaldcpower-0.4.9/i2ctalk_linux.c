/* 
 * vim: set sw=8 ts=8 si et :
 * Copyright Guido Socher */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "i2c_m.h"


#if defined (__APPLE__) && defined (__MACH__)
#define mac 1
#endif

#ifdef sun
	// solaris
static char *port="/dev/ttya";
#endif

#ifdef linux
	// linux
static char *port="/dev/ttyS0";
#endif

#ifdef mac
       // mac OSX with a usb to rs232 converter (pl2303):
static char *port="/dev/tty.usbserial";
#endif

// defines known to worK with gcc: __FreeBSD__ __linux__ __APPLE__ __MACH__ 
//                                 linux sun hpux 
// You can see them with the command: touch foo.h; cpp -dM foo.h

void help(){
	printf("i2ctalk -- send and receive strings over I2C\n");
	printf("Usage: i2ctalk [-h][-d devId][-p device] commandstring\n");
	printf("Options: -h this help\n");
	printf("         -d device Id, integer from 1-254, default is 3\n");
	printf("         -p set the port, default is %s\n",port);
	printf("\n");
	printf("commandstring is the string that will be sent to \n");
	printf("the microcontroller.");
	printf("The program prints any results on stdout.\n");
	exit(1);
}

int main(int argc, char **argv)
{
	/* The following things are used for getopt: */
	extern char *optarg;
	extern int optind;
	extern int opterr;
	int ch;
	int devadr=3;
	char data[MAX_BUF_LEN+1];

	opterr = 0;
	while ((ch = getopt(argc, argv, "hd:p:")) != -1) {
		switch (ch) {
			case 'p':
				port = optarg;
				break;
			case 'd':
				sscanf(optarg,"%d",&devadr);
				break;
			case 'h':
				help();
			case '?':
				fprintf(stderr, "ERROR: No such option. -h for help.\n");
				exit(1);
			/*no default action for case */
			}
	}
	if (optind != argc -1){
		/* exactly one argument must be given */
		help();
	}

	/* open device */
	open_tty(port);
	if (!address_slave(devadr,0)){
		printf("ERROR: address_slave for write failed\n");
		i2cstop();
		close_tty();
		exit(1);
	}
	i2c_tx_string(argv[optind]);
	i2cstop();
        // On the mac we use a usb to serial adapter which
        // unfortunately caches data. We close and re-open to flash.
        // Tested with a pl2303 from http://tech.prolific.com.tw/
	close_tty();
	open_tty(port);
	if (!address_slave(devadr,1)){
		printf("ERROR: address_slave for read failed\n");
		i2cstop();
		close_tty();
		exit(1);
	}
	i2c_rx_string(data);
	printf("%s\n",data);
	i2cstop();
	close_tty();
	return(0);
}


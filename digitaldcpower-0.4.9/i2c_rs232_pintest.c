/* 
 * vim: set sw=8 ts=8 si :
 * program to test the pins on the rs232 port */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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

static int opt_c=0;
static int opt_d=0;

void help(){
	printf("Test the RTS (SCL=-c) and DTR (=SDA=-d) pins on the rs232 port\n");
	printf("Usage: i2c_rs232_pintest [-d 1|0] [-c 1|0] [-p device] \n");
	printf("Options: -d set or clear DTR for 5 sec\n");
	printf("         -c set or clear RTS for 5 sec\n");
	printf("         -p set the port, default is %s\n",port);
	printf("Detection of SDA is done via the CD pin. (connect CD=pin 1 and DTR=pin4)\n");
	exit(1);
}

int main(int argc, char **argv)
{
	/* The following things are used for getopt: */
	extern char *optarg;
	extern int optind;
	extern int opterr;
	int ch;

	opterr = 0;
	while ((ch = getopt(argc, argv, "hd:c:p:")) != -1) {
		switch (ch) {
			case 'p':
				port = optarg;
				break;
			case 'd':
				if (*optarg == '0'){
					opt_d=1;
				}else if (*optarg == '1'){
					opt_d=2;
				}else{
					help();
				}
				break;
			case 'c':
				if (*optarg == '0'){
					opt_c=1;
				}else if (*optarg == '1'){
					opt_c=2;
				}else{
					help();
				}
				break;
			case 'h':
				help();
			case '?':
				fprintf(stderr, "ERROR: No such option. -h for help.\n");
				exit(1);
			/*no default action for case */
			}
	}
	if (optind != argc ){
		/* exactly one argument must be given */
		help();
	}
	if (opt_c ==0 && opt_d ==0){
		help();
	}

	/* open device */
	open_tty(port);
	printf("OK port open\n");
	fflush(stdin);
	scl_low();
	sda_low();
	if (opt_c == 1){
		printf ("OK scl_low\n");
		fflush(stdin);
		scl_low();
	}
	if (opt_c == 2){
		printf ("OK scl_high\n");
		fflush(stdin);
		scl_high();
	}
	if (opt_d == 1){
		printf ("OK sda_low\n");
		fflush(stdin);
		sda_low();
	}
	if (opt_d == 2){
		printf ("OK sda_high\n");
		fflush(stdin);
		sda_high();
	}
	sleep(1);
	printf("SDA is %d\n",sda_value());
	fflush(stdin);
	sleep(4);
	close_tty();
	return(0);
}


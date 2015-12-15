# makefile for windows
all: i2ctalk_win.exe i2c_rs232_pintest.exe
#-------------------
i2c_m.o: i2c_m.h i2c_m.c
	gcc -Wall -c i2c_m.c
i2ctalk_win.o: i2c_m.h i2ctalk_win.c 
	gcc -Wall -c i2ctalk_win.c
i2c_rs232_pintest.o: i2c_m.h i2c_rs232_pintest.c 
	gcc -Wall -c i2c_rs232_pintest.c
#
i2ctalk_win.exe: i2c_m.o i2ctalk_win.o 
	gcc -Wall -o i2ctalk i2c_m.o i2ctalk_win.o
i2c_rs232_pintest.exe: i2c_m.o i2c_rs232_pintest.o 
	gcc -Wall -o i2c_rs232_pintest i2c_m.o i2c_rs232_pintest.o
#-------------------
clean:
	rm -f *.o i2c_rs232_pintest.exe i2ctalk_win.exe
#-------------------

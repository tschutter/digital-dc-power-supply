# makefile, written by guido socher
MCU=atmega8
CC=avr-gcc
OBJCOPY=avr-objcopy
# optimize for size:
CFLAGS=-g -mmcu=$(MCU) -Wall -Wstrict-prototypes -Os -mcall-prologues
#-------------------
all: ddcp.hex i2c_rs232_pintest i2ctalk
#-------------------
help: 
	@echo "Usage: make help"
	@echo "       Print this help"
	@echo " "
	@echo "Usage: make all|load|load_pre|rdfuses|wrfuse1mhz|wrfuse4mhz"
	@echo "       program using the avrdude programmer"
	@echo " "
	@echo "Usage: make all|loaduisp"
	@echo "       program using uisp programmer"
	@echo " "
	@echo "Usage: make clean"
	@echo "       delete all generated files except the pre-compiled ones"
#-------------------
ddcp.hex : ddcp.out 
	$(OBJCOPY) -R .eeprom -O ihex ddcp.out ddcp.hex 
	avr-size ddcp.out
	@echo " "
	@echo "Expl.: data=initialized data, bss=uninitialized data, text=code"
	@echo " "
ddcp.out : ddcp.o lcd.o analog.o dac.o kbd.o i2c_avr.o
	$(CC) $(CFLAGS) -o ddcp.out -Wl,-Map,ddcp.map ddcp.o lcd.o analog.o dac.o kbd.o i2c_avr.o
ddcp.o : ddcp.c lcd.h analog.h hardware_settings.h avr_compat.h kbd.h
	$(CC) $(CFLAGS) -Os -c ddcp.c
#-------------------
lcd.o : lcd.c lcd.h lcd_hw.h
	$(CC) $(CFLAGS) -Os -c lcd.c
#-------------------
i2c_avr.o : i2c_avr.c i2c_avr.h avr_compat.h
	$(CC) $(CFLAGS) -Os -c i2c_avr.c
#-------------------
analog.o : analog.c analog.h avr_compat.h hardware_settings.h
	$(CC) $(CFLAGS) -Os -c analog.c
#-------------------
dac.o : dac.c dac.h avr_compat.h
	$(CC) $(CFLAGS) -Os -c dac.c
#-------------------
kbd.o : kbd.c kbd.h avr_compat.h
	$(CC) $(CFLAGS) -Os -c kbd.c
#-------------------
load: ddcp.hex
	./prg_load_uc ddcp.hex
# here is a pre-compiled version in case you have trouble with
# your development environment
load_pre: ddcp_pre.hex
	./prg_load_uc ddcp_pre.hex
#
loaduisp: ddcp.hex
	./prg_load_uc -u ddcp.hex
# here is a pre-compiled version in case you have trouble with
# your development environment
load_preuisp: ddcp_pre.hex
	./prg_load_uc -u ddcp_pre.hex
#-------------------
# fuse byte settings:
#  Atmel AVR ATmega8 
#  Fuse Low Byte      = 0xe1 (1MHz internal), 0xe3 (4MHz internal), 0xe4 (8MHz internal)
#  Fuse High Byte     = 0xd9 
#  Factory default is 0xe1 for low byte and 0xd9 for high byte
# Check this with make rdfuses
rdfuses:
	./prg_fusebit_uc -r
# use internal RC oscillator 1 Mhz
wrfuse1mhz:
	./prg_fusebit_uc -w 1
# use internal RC oscillator 4 Mhz
wrfuse4mhz:
	./prg_fusebit_uc -w 4
#-------------------
i2c_m.o: i2c_m.h i2c_m.c
	gcc -Wall -c i2c_m.c
i2ctalk_linux.o: i2c_m.h i2ctalk_linux.c 
	gcc -Wall -c i2ctalk_linux.c
i2c_rs232_pintest.o: i2c_m.h i2c_rs232_pintest.c 
	gcc -Wall -c i2c_rs232_pintest.c
#
i2ctalk: i2c_m.o i2ctalk_linux.o 
	gcc -Wall -o i2ctalk i2c_m.o i2ctalk_linux.o
i2c_rs232_pintest: i2c_m.o i2c_rs232_pintest.o 
	gcc -Wall -o i2c_rs232_pintest i2c_m.o i2c_rs232_pintest.o
#-------------------
clean:
	rm -f *.o *.map *.out *t.hex i2c_rs232_pintest i2ctalk
#-------------------

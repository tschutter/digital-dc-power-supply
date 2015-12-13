This directory contains executables for mac OSX (compiled
under mac OS 10.4, Tiger). The source code is the same as for
linux.

There is no special installation package. You can just 
copy them somewhere (e.g home directory) and then run them
from there:
cp i2c_rs232_pintest i2ctalk ~/


Using i2ctalk:
--------------
Print current values:
./i2ctalk i

Print voltage values:
./i2ctalk i

Print set voltage to 4.5V:
./i2ctalk u=45

Print set current to 0.1A:
./i2ctalk u=10

Store settings permanetly:
./i2ctalk s


The mac does these days not have a rs232 interface. You can use
a usb to rs232 "cable". 
Prolific Technology has a devices called PL-2303 USB serial controller:
http://tech.prolific.com.tw/
They have also a driver for mac OSX (md_pl2303H_HX_X_v1.0.9b6.zip). 
This driver can be used by other devices using the same chip
such as: 
ATEN UC-232A http://www.aten-usa.com/
Iogear GUC232A http://www.iogear.com/main.php

You will need to adopt vendor and product id and then you can use
the same kext:
After installing the package, go to the directory
/System/Library/Extensions/ProlificUsbSerial.kext. Edit the
Contents/Info.plist file. Change the idProduct and idVendor numbers to match
those you get from the Apple System Profiler (Note: System Profiler shows
the values in hex and you must enter decimal values in this file).
After that kextload ProlificUsbSerial.kext and check that /dev/tty.usbserial 
shows up whenever the usb to rs232 dongle is plugged in.

The pl2303 is what I used. There are other chips which offer similar
functionallity and they might work. It is just that I have tested
the program with a pl2303 based usb to rs232 "cable".

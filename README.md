# usbturret
Developpement information:

 * *Current version:* 0.1 - might still be slightly buggy
 * *Authors:* Thomas Maurice and Benoit Maliar
 * *License:* WTF Public License v2

## What is this ?
This software is a simple user-mode "driver" for the Tenx
USB missile launchers ! It enables you to drive up to 4 turrets
with an Arduino as a joystick (or basically any device able to
send you info with the correct protocol on the serial port)

## Sounds cool, how do I compile it ?
This is rather simple, all you need is:
 
 * libusb-1.0 developpement files
 * cmake
 * gcc
 * an USB turret

Just run the following commands in the Software/ directory:
    
		mkdir build
		cd build
		cmake ..
		make

## Didn't you mention some Arduino stuff ?
Actually yes ! You also have to compile the software
designed for the arduino and burn it onto the chip.
To do that just go in the Arduino/ folder and run

    make

Yep, no ```cmake``` this time ! Then run

    make upload

to flash the program into the Arduino. The you can
connect the Arduino to the PC and run the main software.

If you have problems uploading the software on the Arduino
please make sure that your user is member of the *dialout*
group. If not run :

    usermod -a G dialout <user>

and logout/login and everything should be fine ! If not, *RTFM* :)

## How to run it ?
Running the software is rather simple, once you've compiled it
you should be able to execute the following command **as the root user**
if you do not do that, it will just fail !
    
		./usbturret [serialport=/dev/ttyACM0]

## Serial protocol
**to be written**

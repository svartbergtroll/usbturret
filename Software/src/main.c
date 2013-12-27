/**
 *            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
 *                 Version 2, December 2004
 *
 *		Copyright (C) 2013 Thomas Maurice <tmaurice59@gmail.com> and Benoit Maliar
 *		 
 * 		Everyone is permitted to copy and distribute verbatim or modified
 * 		copies of this license document, and changing it is allowed as long
 *		as the name is changed.
 * 
 *				   DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
 * 		  TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION
 *	 
 *		 0. You just DO WHAT THE FUCK YOU WANT TO.
 */

/**
 * \file main.c
 * \author Thomas Maurice, Benoit Maliar
 * 
 * \brief Management program for the USB turret
 * \version 0.1
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include <libusb-1.0/libusb.h>

#include <turret.h>
#include <serialcom.h>

/** Serial port used by the program */
#define SERIAL_PORT "/dev/ttyACM0"

/**
 * \brief main, lol
 */
int main(int argc, char ** argv) {

	int serial_fd; //device's pointer for Arduino
	
	// Initialisation of Arduino' link
	printf("Initializing the Arduino...\n");
	serial_fd = init_serial(SERIAL_PORT,SPEED);
	 
	printf("Initializing the library...\n");
	libusb_context *context = newContext();
	
	int currentDevice = 0; //!< Current USB turret enumerated
	libusb_device_handle *handles[4]; //!< Handle array, shall contain all the four turrets (Like "Gilles")
	//!< -> (gilles of the turret, lol)
	for(int i = 0; i < 4; i++) handles[i] = NULL; // Init to NULL just in case we have < 4 devices
	
	libusb_device **list; //!< List of USB devices connected
	size_t count = libusb_get_device_list(context, &list); // fills in device list
	if(count<0) {
		perror("libusb_get_device_list");
		libusb_exit(context); // Clean exit
		exit(-1);
	}
	
	for(size_t i = 0; i < count; i++){
		libusb_device *device = list[i];
		struct libusb_config_descriptor *configDescriptor;
		struct libusb_device_descriptor desc;
		int status = libusb_get_device_descriptor(device,&desc); // 0 on success
		if(status != 0)
			continue;
		
		uint8_t address = libusb_get_device_address(device);
		
		if(desc.idVendor == VENDOR_TURRET && desc.idProduct == PRODUCT_TURRET) {
			configureTurret(device, &handles[currentDevice]);
			
			currentDevice++;
		}
	}
	
	char c = 0;
			
	while(1) {
		unsigned char c = serial(serial_fd);
		int fire = c&0x08;
		int d = c&0x3;
		int order = (c>>4);
		//printf("%x %d\n", c, d);
		if(handles[d] == NULL) continue;
		switch(order) {
			case 0x0: send_command(handles[d], CMD_STOP); break;
			case 0x8: send_command(handles[d], CMD_UP); break;
			case 0x4: send_command(handles[d], CMD_DOWN); break;
			case 0x2: send_command(handles[d], CMD_RIGHT); break;
			case 0x1: send_command(handles[d], CMD_LEFT); break;
		}

		if(fire != 0) // FIRE !
			send_command(handles[d], CMD_FIRE);
		usleep(50000);
	}
			
	for(int i = 0; i < 4; i++)
		releaseHandle(handles[i]);
	
	libusb_free_device_list(list,1);
	libusb_exit(context);
	//Closing Arduino's link
	close_serial(serial_fd);
	
	return EXIT_SUCCESS;
}

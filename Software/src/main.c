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
#include <signal.h>
#include <string.h>

#include <libusb-1.0/libusb.h>

#include <turret.h>
#include <serialcom.h>

/** Serial port used by the program */
#define SERIAL_PORT "/dev/ttyACM0"

/** Main loop indicator, will be set to 0 if we catch ctrl-c */
int main_loop_run = 1;

/**
 * Signal handler so that we can exit the software
 * properly with ctrl-c. This will clear main_loop_run and
 * terminate the main loop of the program. So that all the allocated
 * stuff can be properly unallocated in order to limitate the
 * memory leaks :-)
 * 
 * \param[in] signo Signal we just caught - not used but in the std prototype
 */
void signal_handler(int signo) {
	printf("Stopped by user (SIGINT)\n");
	main_loop_run = 0;
}

/**
 * \brief main, lol
 */
int main(int argc, char ** argv) {
	char port[256]; //!< serial port location
	int serial_fd; //!< serial port file descriptor
	int currentDevice = 0; //!< Current USB turret enumerated
	libusb_device_handle *handles[4]; //!< Handle array, shall contain all the four turrets (Like "Gilles")
	//!< -> (gilles of the turret, lol)
	libusb_device **list; //!< List of USB devices connected
	
	if(argc != 2)
		strncpy(port, SERIAL_PORT, 256); // strncpy to avoid buffer overflows :-)
	else
		strncpy(port, argv[1], 256);
	
	// Initializing signals
	printf("Initializing SIGINT signal handler...\n");
	if (signal(SIGINT, signal_handler) == SIG_ERR) {
        printf(" > Warning, I won't be able to catch SIGINT\n");
	}
	
	// Initialisation of Arduino' link
	printf("Initializing the Arduino on port %s...\n", port);
	serial_fd = init_serial(port,SPEED/* defined in serialcom.h */);
	 
	printf("Initializing the library...\n");
	libusb_context *context = newContext();
	
	for(int i = 0; i < 4; i++) handles[i] = NULL; // Init to NULL just in case we have < 4 devices
	// NULL devices shall never be processed
	
	size_t count = libusb_get_device_list(context, &list); // fills in device list
	if(count<0) {
		perror("libusb_get_device_list");
		libusb_exit(context); // Clean exit
		exit(-1);
	}
	
	for(size_t i = 0; i < count; i++){ // Enumerate and configure the devices
		libusb_device *device = list[i];
		struct libusb_config_descriptor *configDescriptor;
		struct libusb_device_descriptor desc;
		int status = libusb_get_device_descriptor(device,&desc); // 0 on success
		if(status != 0)
			continue;
		
		uint8_t address = libusb_get_device_address(device);
		
		if(desc.idVendor == VENDOR_TURRET && desc.idProduct == PRODUCT_TURRET) {
			configureTurret(device, &handles[currentDevice]); // Configure the device
			
			currentDevice++;
		}
	}
	
	// NOW the fun can begin !
	
	char c = 0;

	while(main_loop_run == 1) { // will be stopped via ctrl-c
		unsigned char c = serial(serial_fd);
		int fire = c&0x08;
		int d = c&0x3;
		int order = (c>>4);
		printf("%x %d\n", c, d);
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
			
		usleep(50000); // Small delay to make the movement consistant and let
		// the Arduino breathe a bit
	}
	
	// Once we've finished let's clean up the mess
	
	for(int i = 0; i < 4; i++)
		releaseHandle(handles[i]);
	
	// Free the list and the lib context
	libusb_free_device_list(list,1);
	libusb_exit(context);
	
	//Closing Arduino's link
	close_serial(serial_fd);
	
	// Finally, return.
	return EXIT_SUCCESS;
}

/* EOF, thanks for reading ;-) */

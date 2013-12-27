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
 * \file turret.h
 * \author Thomas Maurice, Benoit Maliar
 * 
 * \brief Turret handling functions prototypes
 * \version 0.1
 */

#ifndef TURRET_HEADER
#define TURRET_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include <libusb-1.0/libusb.h>

/** Informations used by the USB device to be recognized */
#define VENDOR_TURRET (0x01130)
#define PRODUCT_TURRET (0x0202)

#ifndef DEBUGMODE
	#define DEBUG()
#else
	#define DEBUG(s) printf("%s line %d : %s\n", __FILE__, __LINE__, s);
#endif


/**
 * \enum commands
 */
enum commands {
	CMD_UP,
	CMD_DOWN,
	CMD_RIGHT,
	CMD_LEFT,
	CMD_FIRE,
	CMD_STOP
};

void send_command(libusb_device_handle* handle, int cmd); //!< Send an order to the turret
libusb_context* newContext(); //!< Returns a new libusb_context*
void configureTurret(libusb_device *device,
	libusb_device_handle** handle); //!< Configures a turret
int releaseHandle(libusb_device_handle* handle); //!< Releases a device handle and free its memory

#endif

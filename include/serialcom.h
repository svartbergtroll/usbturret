/**
 *            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
 *                 Version 2, December 2004
 *
 *		Copyright (C) 2004 Sam Hocevar <sam@hocevar.net>
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
 * \file serialcom.h
 * \author Thomas Maurice
 * 
 * \brief Serial port management prototypes
 * \version 0.1
 */

#ifndef SERIALCOM_H
#define SERIALCOM_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>

#define SPEED 	9600

struct termios saveterm;
unsigned char serial(int fd);
// Serial port initialisation
int init_serial(char *device,int speed);
// Close serial port

void close_serial(int fd);
#endif

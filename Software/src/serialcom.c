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
 * \author Thomas Maurice, Benoit Maliar
 * 
 * \brief Serial port management
 * \version 0.1
 */

#include <serialcom.h>

unsigned char serial(int fd){
	unsigned char buffer[1];
	buffer[0] = 0;
	int test;

	//Sending
	write(fd," ",1);
	
	//Reading
	read(fd, buffer,1);
	
	//Printing
//	printf("Command :%x\n", buffer[0]);
	
	return(buffer[0]);
}

// Serial port initialisation
int init_serial(char *device,int speed){
	struct termios new;
	int fd=open(device,O_RDWR|O_NOCTTY);
	if(fd<0){perror(device); exit(-1);}
	tcgetattr(fd,&saveterm); /* save current port settings */
	bzero(&new,sizeof(new));
	new.c_cflag=CLOCAL|CREAD|speed|CS8;
	new.c_iflag=0;
	new.c_oflag=0;
	new.c_lflag=0;  /* set input mode (non-canonical, no echo,...) */
	new.c_cc[VTIME]=0; /* inter-character timer unused */
	new.c_cc[VMIN]=1; /* blocking read until 1 char received */
	tcflush(fd, TCIFLUSH);
	tcsetattr(fd,TCSANOW,&new);
	return fd;
}

// Close serial port

void close_serial(int fd){
	tcsetattr(fd,TCSANOW,&saveterm);
	close(fd);
}

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
 * \file main.c
 * \author Thomas Maurice
 * 
 * \brief Arduino-joystick main program
 * \version 0.1
 * 
 * This program will catch all the Joystick movements
 * (joystick shield from Sparkfun) and the button presses
 * and convert them into orders to drive up to four (4!!!)
 * USB missile launching turrets (Tenx). The program will wait
 * until the computer writes a byte on the serial link (any byte, btw)
 * and send a reply containing orders for the turrets.
 * 
 * These orders are in one byte:
 *    UP | DOWN | RIGHT | LEFT | FIRE | TURSEL[2:0]
 * Where UP, DOWN, RIGHT, LEFT and FIRE are 0 or 1 depending of what
 * you want to do, and TURSEL[2:0] is 0, 1, 2, 3 depending on the
 * turret you want to drive.
 */
 
#include <avr/io.h>		// for the input/output register
#include <avr/interrupt.h> 	
#include <util/delay.h>
#include <stdlib.h>

/** Baudrate for the serial link */
#define BAUDRATE	103 // UBRR value for 9600

typedef uint8_t u8;

/**
 * \union Register
 * \brief Union used to split bytes into bits
 * 
 * It can also split it into 4-bit long words via high and low.
 * All the bits are accessed via b0...7
 */
typedef union {
	u8		 byte;
	struct {
		u8 low	:4;
		u8 high :4;
	};
	struct {
		u8 b0 :1;
		u8 b1 :1;
		u8 b2 :1;
		u8 b3 :1;
		u8 b4 :1;
		u8 b5 :1;
		u8 b6 :1;
		u8 b7 :1;
	};
} Register;

// Turrets definitions
#define T1 0x1
#define T2 0x2
#define T3 0x4
#define T4 0x8

u8 currentTurret;

// For the AD converter
void ad_init(unsigned char channel)   
{   
	ADCSRA|=(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);   
	ADMUX|=(1<<REFS0)|(1<<ADLAR);
	ADMUX=(ADMUX&0xf0)|channel;   
	ADCSRA|=(1<<ADEN);
}   

void init_serial(void) {
	/* ACHTUNG : we suppose UBRR value < 0xff */
	/* Not true in all case */
	uint8_t baudrate = BAUDRATE;
	/* Set baud rate */
	UBRR0H = 0;
	UBRR0L = baudrate;

	/* Enable transmitter */
	UCSR0B = (1<<TXEN0|1<<RXEN0);

	/* Set frame format */
	UCSR0C = 0x06;
}

void send_serial(u8 c) {
	while(!(UCSR0A&(1<<UDRE0)));
	UDR0=c;
}

// Get a sample from the ATD
unsigned int ad_sample(void){
	ADCSRA|=(1<<ADSC);
	while(bit_is_set(ADCSRA, ADSC));
	return ADCH;
}

void output_init(void){
	DDRB |= 0x1F; // PORTB[4..0]
}

void input_init(void){
	DDRD &= 0x0;  // PORTD as input
	PORTD |= 0xFF; // Pull-up activated on PORTD
}

int main(void){
	// Initializing the Arduino
	output_init();
	input_init();
	init_serial();
	
	// Default setting
	currentTurret = T1;
	
	// Unionize the portb and d for easy access
	Register *portb = (Register*)(&PORTB);
	Register *portd = (Register*)(&PIND);
	
	// Command byte
	Register cmd;
	
	while(1) {
		cmd.byte = 0;
		/*
		 * Check if the user wants to change 
		 * which turret he is playing with
		 */
		if(portd->b6 == 0)
			currentTurret = T4;
		else if(portd->b5 == 0)
			currentTurret = T2;
		else if(portd->b4 == 0)
			currentTurret = T3;
		else if(portd->b3 == 0)
			currentTurret = T1;
		// For the "fire" button
		else if(portd->b2 == 0) {
			for(u8 i = 0; i < 10; i++) {
				portb->byte ^= 0x0F;
				_delay_ms(100);
			}
			cmd.b3 = 1;
		}
		
		/*
		 * Set the direction part of the byte
		 * The high bits of the command byte.
		 */
		ad_init(0);
		u8 s = ad_sample();
		
		if(s <= 132 && s >= 122) {
			// Milieu
		} else if(s > 132) {
			// Droite 
			cmd.high |= 0x2;
		} else if(s < 122) {
			// Gauche
			cmd.high |= 0x1;
		}
		
		ad_init(1);
		s = ad_sample();
		if(s <= 132 && s >= 122) {
			// Milieu
		} else if(s > 132) {
			cmd.high |= 0x8;
		} else if(s < 122) {
			cmd.high |= 0x4;
		}
		
		/*
		 * Set the low part of the command byte: TURSEL[2..0]
		 */
		switch (currentTurret) {
			case T1:
				cmd.low |= 0x0;
				break;
			case T2:
				cmd.low |= 0x1;
				break;
			case T3:
				cmd.low |= 0x2;
				break;
			case T4:
				cmd.low |= 0x3;
				break;
		}
		
		portb->b4 = 1; // Turn the red LED on
		portb->low = currentTurret; // Show which turret we are controlling
		send_serial(cmd.byte); // Send the command byte to the computer
		while(!(UCSR0A&(1<<RXC0)));
		u8 r = UDR0; // Read the byte to reset the flag
		portb->b4 = 0; // Turn the red LED off
	}
	
	return 0;
}

//Lab 7 Code for the Receiver Microcontroller
//Written by Trevor Kickliter
//Last updated: 3/26/2020
#include <avr/io.h>

//** GLOBAL VARIABLES **/
char sensorvalue = 0; // value read from analog sensor (0-255)
int data;

//** FUNCTION DECLARATIONS **//
void initialize_usart(void);

int main(void)
{
	//Set up data registers
	DDRC = 0xFF; //Defines all C ports as output

	//Set up USART
	initialize_usart();

	//Program loop
	while(1) {
	   
		//Read data from transmitter µc
		data = UDR0;
		PORTC = (data & 0b11111100)/4; //Displays top 6 bits on bottom 6 bits of port C

	} // end main while

} // end main

void initialize_usart(void) { // function to set up USART
	UCSR0B = (1<<RXEN0); // enable serial transmission
	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00) ; // Asynchronous mode, 8‐bit
	// data; no parity; 1 stop bit
	UBRR0L = 0x67; // 9,600 baud if Fosc = 16MHz
}
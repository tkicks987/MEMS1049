//** GLOBAL VARIABLES **
	/*
 * adc_example.c
 *
 * Created: 3/20/2016 3:52:50 PM
 * Author : WClark
 */


/* This program is an example C code for A-to-D conversion using the Atmel chip.
An analog voltage is read (e.g. from potentiometer).
The binary value of the high byte (bits 2-9 of the 10-bit converted value) are echoed out PORTD to be displayed on LEDs.
*/
#include <avr/io.h>
#include <avr/interrupt.h>

//** GLOBAL VARIABLES **/
long freqclk = 16000000; // clock frequency in Hz
char ledNum = 0;
char sensorvalue = 0; // value read from analog sensor (0-1023)
int phase_step = 1;
int position = 0;
int setpoint = 0;

//** FUNCTION DECLARATIONS **//
void wait(volatile int multiple, volatile char time_choice);
void delay_T_msec_timer0(char choice);
void step_CW( void );
void step_CCW( void );

int main(void)
{

	// Setup
	DDRD = 0xFF; // Sets all pins of Port D to output.

	// Set up ADC
	DDRC =  0x00;  // define all Port C bits as input
	PRR = 0x00;  // clear Power Reduction ADC bit (0) in PRR register
	ADCSRA = 1<<ADEN | 1<<ADPS2 | 1<<ADPS1 | 1<<ADPS0;  // 0x87 // 0b10000111 // Set ADC Enable bit (7) in ADCSRA register, and set ADC prescaler to 128 (bits 2-0 of ADCSRA = ADPS2-ADPS0 = 111)
	ADMUX = 0<<REFS1 | 1<<REFS0 | 1<<ADLAR; //0x60; // 0b01100000  // select Analog Reference voltage to be AVcc (bits 7-6 of ADMUX = 01),
	//left justification (bit 5 of ADMUX = ADLAR = 1);
	//and select channel 0 (bits 3-0 of ADMUX = MUX3-MUX0 = 000)
	   
	while(1) {

		//Script for testing stepping
		/*step_CW();
		wait(250, 2);*/

		//Full script

	    // Read analog input
		ADCSRA |= (1<<ADSC); // Start conversion
		while ((ADCSRA & (1<<ADIF)) == 0); // wait for conversion to finish
		sensorvalue = ADCH; // Keep high byte of 10-bit result (throw away lowest two bits)
		
		//Determines necessary position and adjusts accordingly 
		setpoint = int(100 * (sensorvalue - 128)/128);
		if (position > setpoint) {
			step_CW();
			wait(100, 2);
			position--;
		}
		else if (position < setpoint) {
			step_CCW();
			wait(100, 2);
			position++;
		}


	} // end main while

} // end main

void step_CW() {
	// This function advances the motor clockwise one step. Follow the
	//full‐wave stepping table in Stepper Motor Control.ppt for MEMS 1049
	//course. Our stepper motor phases are assigned to Port pins as
	//follows: phase1a = PORTD_7, phase1b = PORTD_6, phase2a = PORTD_5,
	//phase2b = PORTD_4
	switch (phase_step) {
		case 1:
			// step to 4
			PORTD = 0b00100000;
			phase_step = 4;
			break;
		case 2:
			// step to 1
			PORTD = 0b10000000;
			phase_step = 1;
			break;
		case 3:
			// step to 2;
			PORTD = 0b00010000;
			phase_step = 2;
			break;
		case 4:
			// step to 3;
			PORTD = 0b01000000;
			phase_step = 3;
			break;
	}
}// end step_CW

void step_CCW()
{
	// This function advances the motor counter‐clockwise one step.
	//Follow the full‐wave stepping table in Stepper Motor Control.ppt
	//for MEMS 1049 course. phase1a = PORTD_7, phase1b = PORTD_6
	// phase2a = PORTD_5, phase2b = PORTD_4
	switch (phase_step) {
		case 1:
			// step to 2
			PORTD = 0b00010000;
			phase_step = 2;
			break;
		case 2:
			// step to 3
			PORTD = 0b01000000;
			phase_step = 3;
			break;
		case 3:
			// step to 4;
			PORTD = 0b00100000;
			phase_step = 4;
			break;
		case 4:
			// step to 1;
			PORTD = 0b10000000;
			phase_step = 1;
			break;
	}
}// end step_CCW

void wait(volatile int multiple, volatile char time_choice) {
//*** wait ***
/* This subroutine calls others to create a delay.
Total delay = multiple*T, where T is in msec and is the delay created by the called function.

Inputs: multiple = number of multiples to delay, where multiple is the number of times an actual delay loop is called.
Outputs: None
*/

	while (multiple > 0) {
		delay_T_msec_timer0(time_choice); // we are choosing case 2, which is a 1 msec delay
		multiple--;
	}
} // end wait()

void delay_T_msec_timer0(char choice) {
    //*** delay T ms **
    /* This subroutine creates a delay of T msec using TIMER0 with prescaler on clock, where, for a 16MHz clock:
    for Choice = 1: T = 0.125 msec for prescaler set to 8 and count of 250 (preload counter with 5)
    for Choice = 2: T = 1 msec for prescaler set to 64 and count of 250 (preload counter with 5)
    for Choice = 3: T = 4 msec for prescaler set to 256 and count of 250 (preload counter with 5)
    for Choice = 4: T = 16 msec for prescaler set to 1,024 and count of 250 (preload counter with 5)
	for Choice = Default: T = .0156 msec for no prescaler and count of 250 (preload counter with 5)

	Inputs: None
	Outputs: None
	*/

	TCCR0A = 0x00; // clears WGM00 and WGM01 (bits 0 and 1) to ensure Timer/Counter is in normal mode.
	TCNT0 = 0;  // preload value for testing on count = 250
	// preload value for alternate test on while loop: TCNT0 = 5;  // preload load TIMER0  (count must reach 255-5 = 250)

	switch ( choice ) { // choose prescaler
		case 1:
			TCCR0B = 0b00000010; //1<<CS01; TCCR0B = 0x02; // Start TIMER0, Normal mode, crystal clock, prescaler = 8
			break;
		case 2:
			TCCR0B =  0b00000011; //1<<CS01 | 1<<CS00; TCCR0B = 0x03;  // Start TIMER0, Normal mode, crystal clock, prescaler = 64
			break;
		case 3:
			TCCR0B = 0b00000100; //1<<CS02; TCCR0B = 0x04; // Start TIMER0, Normal mode, crystal clock, prescaler = 256
			break;
		case 4:
			TCCR0B = 0b00000101; //1<<CS02 | 1<<CS00; TCCR0B = 0x05; // Start TIMER0, Normal mode, crystal clock, prescaler = 1024
			break;
		default:
			TCCR0B = 0b00000001; //1<<CS00; TCCR0B = 0x01; Start TIMER0, Normal mode, crystal clock, no prescaler
			break;
	}

	while (TCNT0 < 0xFA); // exits when count = 250 (requires preload of 0 to make count = 250)
	// alternate test on while loop: while ((TIFR0 & (0x1<<TOV0)) == 0); // wait for TOV0 to roll over:
	// How does this while loop work?? See notes

	TCCR0B = 0x00; // Stop TIMER0
	//TIFR0 = 0x1<<TOV0;  // Alternate while loop: Clear TOV0 (note that this is a nonintuitive bit in that it is cleared by writing a 1 to it)

} // end delay_T_msec_timer0()
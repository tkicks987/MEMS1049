//Lab 5 Code
//By: Trevor Kickliter

//** GLOBAL VARIABLES **/
long freqclk = 16000000; // clock frequency in Hz
char ledNum = 0;
char sensorvalue = 0; // value read from analog sensor (0-1023)
char high = 0; //Forward out write value
char low = 0; //Backward out write value

//** FUNCTION DECLARATIONS **//
void wait(volatile int multiple, volatile char time_choice);
void delay_T_msec_timer0(char choice);

#include <avr/io.h>
#include <avr/interrupt.h>

int main(void)
{
	// Setup
	DDRD = 0xFF; // Sets all pins of Port D to output.
	DDRB = 0xFF;

	// Set up ADC
	DDRC =  0x00;  // define all Port C bits as input
	PRR = 0x00;  // clear Power Reduction ADC bit (0) in PRR register
	ADCSRA = 1<<ADEN | 1<<ADPS2 | 1<<ADPS1 | 1<<ADPS0;  // 0x87 // 0b10000111 // Set ADC Enable bit (7) in ADCSRA register, and set ADC prescaler to 128 (bits 2-0 of ADCSRA = ADPS2-ADPS0 = 111)
	ADMUX = 0<<REFS1 | 1<<REFS0 | 1<<ADLAR; //0x60; // 0b01100000  // select Analog Reference voltage to be AVcc (bits 7-6 of ADMUX = 01),
	//left justification (bit 5 of ADMUX = ADLAR = 1);
	//and select channel 0 (bits 3-0 of ADMUX = MUX3-MUX0 = 000)

	//Set up timer
	TCCR0A = 1<<COM0A1 | 0<<COM0A0 | 1<<WGM01 | 1<<WGM00;      // Set non-inverting mode on OC0A pin (COMA1:0 = 10; Fast PWM (WGM1:0 bits = bits 1:0 = 11) (Note that we are not affecting OC0B because COMB0:1 bits stay at default = 00)
	TCCR0B = 0<<CS02 | 1<<CS01 | 1<<CS00;
   
	while(1) {
	   
		// Read analog input
		ADCSRA |= (1<<ADSC); // Start conversion
		while ((ADCSRA & (1<<ADIF)) == 0); // wait for conversion to finish

		sensorvalue = ADCH; // Keep high byte of 10-bit result (throw away lowest two bits)
		if (sensorvalue < 50) {
			PORTB = 0b1111011;
			PORTD = 0b1111110;
			OCR0A = 255;
		}
		if ((sensorvalue > 50) && (sensorvalue < 100)) {
			PORTB = 0b1111011;
			PORTD = 0b1111101;
			OCR0A = 200;
		}
		if ((sensorvalue > 101) && (sensorvalue < 150)) {
			PORTB = 0b1111111;
			PORTD = 0b1111011;
			OCR0A = 0;
		}
		if ((sensorvalue > 151) && (sensorvalue < 200)) {
			PORTB = 0b1111101;
			PORTD = 0b1110111;
			OCR0A = 200;
		}
		if ((sensorvalue > 201) && (sensorvalue < 256)) {
			PORTB = 0b1111101;
			PORTD = 0b1101111;
			OCR0A = 255;
		}

	} // end main while

} // end main

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
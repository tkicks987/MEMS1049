//Lab 3 Code
//By: Trevor Kickliter

//** GLOBAL VARIABLES **/
long freqclk = 16000000; // clock frequency in Hz
char ledNum = 0;

//** FUNCTION DECLARATIONS **//
void wait(volatile int multiple, volatile char time_choice);
void delay_T_msec_timer0(char choice);

#include <avr/io.h>
#include <avr/interrupt.h>

int main(void)
{
	DDRD = 0x00;  //Sets all pins as input
	DDRC = 0xFF; //Sets all pins as output
	PORTC = 0xFF; // set all bits on PORTD so that all output are off (assumes circuit is wired as active low)
    
	//Sets interrupts
	EICRA = 1<<ISC01 | 1<<ISC00 | 1<<ISC11 | 1<<ISC10;	// Trigger INT0 and INT1 on rising edge
	EIMSK = 1<<INT1 | 1<<INT0;	// Enable INT1 and INT0
	sei();	//Enable Global Interrupt
	
	//Main loop
	while(1)
    {
		//Cycle through  outputs
		if (ledNum == 0) {
			PORTC = 0b11110111;
			wait(1000,2);
			ledNum = 1;
		}
		else if (ledNum == 1) {
			PORTC = 0b11111011;
			wait(1000,2);
			ledNum = 2;
		}
		else if (ledNum == 2) {
			PORTC = 0b11111101;
			wait(1000,2);
			ledNum = 3;
		}
		else {
			PORTC = 0b11111110;
			wait(1000,2);
			ledNum = 0;
		}
	}
	
return 0;	
} // end main

/* ============================================
  FUNCTIONS
 ============================================
*/

//Interrupts:

//Interrupt for PD2 (INT0)
ISR(INT0_vect) {
	
	//Cycles through routine
	for (int i = 0; i < 16; i++) {
		
		PORTC = 0x00 + i;
		wait(250,2);
		
	}
	
}

//Interrupt for PD3 (INT1)
ISR(INT1_vect) {
	
	//Turns everything on for 2 seconds
	PORTC = 0x00;
	wait(2000,2);
	
}

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
			TCCR0B = 0b00000010; //1<<CS01;	TCCR0B = 0x02; // Start TIMER0, Normal mode, crystal clock, prescaler = 8
		break;
		case 2:
			TCCR0B =  0b00000011; //1<<CS01 | 1<<CS00;	TCCR0B = 0x03;  // Start TIMER0, Normal mode, crystal clock, prescaler = 64
		break;
		case 3:
			TCCR0B = 0b00000100; //1<<CS02;	TCCR0B = 0x04; // Start TIMER0, Normal mode, crystal clock, prescaler = 256
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
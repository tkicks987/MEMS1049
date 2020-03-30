//MEMS 1049 Lab 2 Code
//By: Trevor Kickliter

//** GLOBAL VARIABLES **/
long freqclk = 16000000; // clock frequency in Hz
char ledNum = 0;

//** FUNCTION DECLARATIONS **//
void wait(volatile int N);  // Note that variables in the delay loops are declared as volatile.  If not, the compiler will likely
// see these as not necessary since they are not used for any important tasks or I/O and will eliminate the loops.  In fact, if
// the simulator is used without these being declared volatile, then the loops will be ignored all together in the simulation,
// and the cycle counter will not grow correctly.
void delay_1_msec_raw_count();

#include <avr/io.h>

int main(void)
{
	DDRD = 0x00;  //Sets all pins as input
	DDRC = 0xFF; //Sets all pins as output
	PORTC = 0xFF; // set all bits on PORTD so that all output are off (assumes circuit is wired as active low)
    // ============================================
    // P R O G R A M L O O P
    // ============================================
	while(1)
    {
		//Check if the switch is open
	   	if (PIND & 0b00000100) // PD2 is open
		{
			//Cycle through  outputs
			if (ledNum == 0) {
				PORTC = 0b11110111;
				wait(1000);
				ledNum = 1;
				continue;
			}
			else if (ledNum == 1) {
				PORTC = 0b11111011;
				wait(1000);
				ledNum = 2;
				continue;
			}
			else if (ledNum == 2) {
				PORTC = 0b11111101;
				wait(1000);
				ledNum = 3;
				continue;
			}
			else {
				PORTC = 0b11111110;
				wait(1000);
				ledNum = 0;
				continue;
			}
		}
		else // Door 1 switch is closed.
		{
			PORTC = 0x00; //Turn on all LEDs
		}
	}
	
return 0;	
} // end main

/* ============================================
  FUNCTIONS
 ============================================
*/

void wait(volatile int N) {
	//*** wait ***
		
	/* This subroutine creates a delay of N msec by calling another function.
			Inputs: N = number of msec for the delay (N is int, so can be up to 65,536 msec)
			Outputs: None
			
		Why is N a volatile variable? Some C compilers try to optimize the code and may eliminate code that appears to do nothing.  A time-killing 
		loop is is one of those code blocks that could be removed during compile.  Using a nonvolatile variable in the function tells the compiler not 
		remove it.
	*/
	while (N>1) {  // This loop creates an N msec delay
		N--;
		delay_1_msec_raw_count();
	}
} // end wait()

void delay_1_msec_raw_count() {
    //*** delay N ms **
    /* This subroutine creates a delay of N msec by using a raw time-killing count.
			Inputs: None
			Outputs: None
	*/
	volatile int count = (freqclk/1000-28)/4; // each cycle takes 4 clock cycles, so count is the number of cycles needed to create a 1 msec delay.
									// 28 is the approximate number of other clock cycles used in the subroutine
	while (count > 1) {  // This loop is expected to create a 1 msec delay, but it is in fact much longer.  Because of the size of the variables,
			// there is a lot more that needs to be done each cycle so the loop takes much longer than 1 msec to execute. To know exact time for each 
			// instruction, use assembly language instead of C. (Or look at the disassembly code file and count instructions.)
			count--; // decrement count
	}
	
		
} // end delay_1_msec_raw_count()
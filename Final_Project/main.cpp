//Code for Mechatronics Project
//By: Trevor Kickliter, London Lee, and Yunan Liu
//Last edited: 3/29/2020
#include <avr/io.h>
#include <stdlib.h>
#include <math.h>

//Function declarations
void wait(volatile int multiple, volatile char time_choice);
void delay_T_msec_timer0(char choice);
void print_message(char string[]);
void display_instructions(void);
void select_difficulty(void);
void easy_game(void);
void hard_game(void);
void press_button(void);
void shout(void);
void rotate(void);
void hard_press(void);
void step_CW(void);
void step_CCW(void);
void pause(void);

//Global variables
long freqclk = 16000000; // clock frequency in Hz
char sensorvalue = 0;
char ledNum = 0;
char difficulty = 0;
char mvmts = 0;
char fails = 0;
bool instruct = 1;
int time_left = 0;
int seed = 0;
char task = 0;
char ledNum = 0;
char ledSeed = 0;
char buttonNum = 0;
int phase_step = 1;
int time_elapsed = 0;
bool pass = 0;
int dir = 0;
//char threshold = 128; Speaker volume must exceed this threshold

//Main function
int main(void)
{
	
	//Initialize data registers
	DDRD = 0xFF;
	DDRC = 0x00;
	DDRB = 0b00101101;
	
	//Initialize port outputs
	PORTC = 0xFE;
	PORTD = 0xFF;
	
	//Set up ADC
	PRR = 0x00;
	ADCSRA = 0x87;
	ADMUX = 0x60;
	
	//Set up SPI
	SPCR = 0b01010001;
	
	//Game loop
	bool repeat = 1;
	while (repeat) {
		
		//Displays instructions
		display_instructions();
		
		//Prompt user to select difficulty
		select_difficulty();
		
		//Secondary game loop (loops until user fails game):
		
		//Initializes settings
		mvmts = 0;
		fails = 0;
		if (difficulty == 1) { //Easy settings
			easy_game();
		}
		else if (difficulty == 2) {
			hard_game();
		}
		
		//Prompts user to repeat
		print_message("GAME OVER!!!\nPress button 1 to try again. Press button 2 to quit");
		while (1) {
			
			wait(100, 2);
			if (!(PINC & 0b00000100)) { //Button 1 is pressed
				repeat = 1;
				wait(200, 2);
				break;
			}
			else if (!(PINC & 0b00001000)) { //Button 2 is pressed
				repeat = 0;
				wait(200, 2);
				break;
			}
			
		} //End repeat selection loop
		
		
	} //End game loop
    
}

//Function for easy mode
void easy_game(void) {
	
	//Loops until failure
	print_message("Playing on easy mode.\nGood luck!")
	time_left = 2000;
	while (1) {
	
		//Generates a random task
		seed = rand() % 100 + 1;
		if (mvmts < 6) {
			
			//Always a random LED flash
			task = 1;
			
		}
		else if (mvmts >= 6 & mvmts < 12) {
			
			//50% chance of either an LED flash or shout command
			task = seed % 2 + 1;
			
		}
		else if (mvmts == 12) {
			
			//Always a platform rotation
			task = 3;
		}
		else {
			
			//40% chance of button press, 40% chance of LED flash, 20% chance of rotation
			if (seed <= 40) {
				task = 1;
			}
			else if ((seed > 40) & (seed <= 80)) {
				task = 2;
			}
			else {
				task = 3;
			}
			
		}
		
		//User must perform task within time allotted
		time_elapsed = 0;
		switch (task) {
			
			case 1: //LED flash / Button press
				
				press_button();
				break;
				
			case 2: //Buzzer / Shout
			
				shout();
				break;
				
			case 3: //Platform rotation
			
				rotate();
				break;
			
		}
		
		//Breaks if there are too many failures
		if (fails == 3) {
			break;
		}
		
		//Decrement time remaining
		mvmts++;
		if (!(mvmts % 4) & time_left > 200) {
			time_left -= 200;
		}
	
	} //End game while
	
}

//Function for hard mode (in progress)
void hard_game(void) {
	
	//Loops until failure
	print_message("Playing on hard mode.\nGood luck!")
	time_left = 1600;
	while (1) {
		
		//Generates a random task
		seed = rand() % 100 + 1;
		if (mvmts < 4) {
			
			//Always a random LED flash
			task = 1;
			
		}
		else if (mvmts >= 4 & mvmts < 7) {
			
			//50% chance of either an LED flash or shout command
			task = seed % 2 + 1;
			
		}
		else if (mvmts == 10) {
			
			//Always a platform rotation
			task = 3;
		}
		else if (mvmts > 10 & mvmts < 20) {
			
			//40% chance of button press, 40% chance of LED flash, 20% chance of rotation
			if (seed <= 35) {
				task = 1;
			}
			else if ((seed > 35) & (seed <= 70)) {
				task = 2;
			}
			else {
				task = 3;
			}
			
		}
		else {
			
			//Button presses are now harder and it's more likely that you'll get them
			if (seed <= 40) {
				task = 4;
			}
			else if ((seed > 40) & (seed <= 70)) {
				task = 2;
			}
			else {
				task = 3;
			}
			
		}
		
		//User must perform task within time allotted
		time_elapsed = 0;
		switch (task) {
			
			case 1: //LED flash / Button press
			
				press_button();
				break;
			
			case 2: //Buzzer / Shout
			
				shout();
				break;
				
			case 3: //Platform rotation
			
				rotate();
				break;
			
			case 4: //Hard button press
				
				hard_press();
				break;
			
			
		}
		
		//Breaks if there are too many failures
		if (fails == 3) {
			break;
		}
		
		//Decrement time remaining
		mvmts++;
		if (!(mvmts % 4) time_left > 200) {
			time_left -= 200;
		}
		
	} //End game while
	
}

//Function to print messages on the OLED board (in progress)
void print_message(char string[]) {
	
	/*Display message to OLED board -- Implemented later*/

}

//Function for pausing
void pause(void) {
	
	//Wait until user presses a button
	while (1) {
			
		//Loop until user presses a button
		wait(100, 2);
		if (!((PINC & 0b00111000) == 0b00111100)) { //Buttons 2-4 (PC3-PC5) are pressed
			instruct = 0;
			wait(200, 2);
			break;
		}
			
	}
		
}

//Function to select difficulty
void select_difficulty(void) {
	
	print_message("Select difficulty: Press button 1 for EASY or button 2 for HARD.");
	while (1) {
		
		wait(100, 2);
		if (!(PINC & 0b00000100)) { //Button 1 is pressed
			difficulty = 1;
			wait(200, 2);
			break;
		}
		else if (!(PINC & 0b00001000)) { //Button 2 is pressed
			difficulty = 2;
			wait(200, 2);
			break;
		}
		
	} //End difficulty loop
	
}

//Function to display instructions
void display_instructions(void) {
	
	while (instruct) {
		
		//Displays instructions
		print_message("Welcome to Mecha Madness!\nPress any button to continue");
		pause();
		print_message("Perform the required action before time runs out.");
		pause();
		print_message("If an LED flashes, press the button coresponding\nto the number the LED is labeled.");
		pause();
		print_message("If you hear a beep, shout into the microphone.");
		pause();
		
		//Prompts user to repeat instructions
		print_message("Press button 1 to repeat instructions. Press any other button to continue.");
		while (1) {
			
			//Loop until user presses a button
			wait(100, 2);
			if (!(PINC & 0b00000100)) { //Button 1 (PC2) is pressed
				instruct = 1;
				wait(200, 2);
				break;
			}
			else if (!((PINC & 0b00111000) == 0b00111000)) { //Buttons 2-4 (PC3-PC5) are pressed
				instruct = 0;
				wait(200, 2);
				break;
			}
			
		}
		
	} //End instruction loop

}

//Function for button press task
void press_button(void) {
	
	//Determines which LED needs to be pressed
	ledSeed = rand() % 4; //Determines which pin (0-3) lights up
	ledNum = !(pow(2, ledSeed));
	buttonNum = ledNum * 4; //Determines which button needs to be pressed
	PORTD = ledNum;
	
	//User must press the assigned number within the allotted time
	while (1) {
		
		//Breaks if task is fulfilled in time or if time runs out
		if (!(PINC & buttonNum)) {
			break;
		}
		if (time_elapsed > time_left) {
			fails++;
			break;
		}
		wait(20, 2);
		time_elapsed += 20;
		
	}
	
}

//Function for more difficult button press
void hard_press(void) {
	
	//Determines which LED needs to be pressed
	ledSeed = rand() % 6;
	switch ledSeed {
		
		case 0:
			ledNum = 0b00000011;
			break;
		case 1:
			ledNum = 0b00000101;
			break;
		case 2:
			ledNum = 0b00001001;
			break;
		case 3:
			ledNum = 0b00000110;
			break;
		case 4:
			ledNum = 0b00001010;
			break;
		case 5:
			ledNum = 0b00001100;
			break;
		
	}
	buttonNum = ledNum * 4;
	PORTD = ledNum;
	
	//User must press the assigned number within the allotted time
	while (1) {
		
		//Breaks if task is fulfilled in time or if time runs out
		if (!(PINC & buttonNum) == buttonNum) {
			break;
		}
		if (time_elapsed > time_left) {
			fails++;
			break;
		}
		wait(20, 2);
		time_elapsed += 20;
		
	}
	
}

//Function for shout task (in progress)
void shout(void) {
	
	//Activates buzzer
	//PORTB = PORTB & 0b00000001; Commented until OLED display is figured out, since it also uses port B
	
	//User must shout within the allotted time
	/* Commented until appropriate threshold is determined
	while (1) {
		
		//Read analog input
		ADCSRA |= (1<<ADSC); // Start conversion
		while ((ADCSRA & (1<<ADIF)) == 0); // wait for conversion to finish
		sensorvalue = ADCH;
		
		//Breaks if task is fulfilled in time or if time runs out
		if (sensorvalue > threshold) {
			break;
		}
		if (time_elapsed > time_left) {
			fails++;
			break;
		}
		wait(20, 2);
		time_elapsed += 20;
		
	}
	*/
	
}

//Function for platform rotation
void rotate(void) {
	
	//Rotates 90 degrees in random direction
	dir = rand() % 1;
	for (int i = 0; i < 50; i++) {
		
		if (dir) {
			step_CW();
			wait(10, 2); //Waits are set such that 90 degree rotation will be achieved in a half second
		}
		else {
			step_CCW();
			wait(10, 2);
		}
		
	}
	
}
	
void step_CW() {
	// This function advances the motor clockwise one step. Follow the
	//full‐wave stepping table in Stepper Motor Control.ppt for MEMS 1049
	//course. Our stepper motor phases are assigned to Port pins as
	//follows: phase1a = PORTD_7, phase1b = PORTD_6, phase2a = PORTD_5,
	//phase2b = PORTD_4
	switch (phase_step) {
		case 1:
			// step to 4
			PORTD = 0b00101111;
			phase_step = 4;
			break;
		case 2:
			// step to 1
			PORTD = 0b10001111;
			phase_step = 1;
			break;
		case 3:
			// step to 2;
			PORTD = 0b00011111;
			phase_step = 2;
			break;
		case 4:
			// step to 3;
			PORTD = 0b01001111;
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
			PORTD = 0b00011111;
			phase_step = 2;
			break;
		case 2:
			// step to 3
			PORTD = 0b01001111;
			phase_step = 3;
			break;
		case 3:
			// step to 4;
			PORTD = 0b00101111;
			phase_step = 4;
			break;
		case 4:
			// step to 1;
			PORTD = 0b10001111;
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


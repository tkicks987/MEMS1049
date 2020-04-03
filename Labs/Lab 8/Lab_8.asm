;MEMS 1049 Lab 8 Code
;By: Trevor Kickliter
;Last updated: 4/2/2020

;Header files 
.NOLIST
.INCLUDE "m328pdef.inc"
.LIST

;SRAM Definitions
.DSEG
.ORG 0x100

;Reset and int vectors
.CSEG
.ORG 0x0000
JMP Main
JMP INT0_vect
JMP INT1_vect

;Main
Main:

	;Initialize stack
	LDI R16, HIGH(RAMEND)
	OUT SPH, R16
	LDI R16, LOW(RAMEND)
	OUT SPL, R16

	;Sets data registers
	LDI R16, 0xFF
	OUT DDRC, R16 //Sets port C as output
	OUT PORTC, R16 //Turns all LEDs off

	;Sets interrupts
	LDI R16, (1<<ISC01) | (1<<ISC00) | (1<<ISC11) | (1<<ISC10)
	STS EICRA, R16
	LDI R16, (1<<INT1) | (1<<INT0)
	OUT EIMSK, R16
	LDI R16, (1<<INTF0) | (1<<INTF1)
	OUT EIFR, R16
	SEI //Global interrupt enable

	;Initializes register 18, which will be used to store the LED number
	LDI R18, 0	

	;Main loop
	Loop_main:

		;Cycles through outputs
		CPI R18, 0
			CALL blink_4th
		CPI R18, 1
			CALL blink_3rd
		CPI R18, 2
			CALL blink_2nd
		CPI R18, 3
			CALL blink_1st
		RJMP Loop_main

//Interrupt for PD2 (INT0)
INT0_vect:

	//Pushes status register onto the stack
	CALL wait
	PUSH R17
	IN R17, SREG
	PUSH R17

	//Cycles through binary combos up to 16
	LDI R23, 0
	LDI R24, 15
	int0_loop:
		INC R23
		OUT PORTC, R23
		CALL wait
		CPSE R23, R24
		RJMP int0_loop

	//Exits interrupt
	LDI R17, 0x01
	OUT EIFR, R17 //Clears INT0 in case
	POP R17
	OUT SREG, R17
	POP R17
	RETI

//Interrupt for PD3 (INT1)
INT1_vect:

	//Pushes status register onto the stack
	PUSH R17
	IN R17, SREG
	PUSH R17

	//Turns everything on for 2 seconds
	LDI R16, 0x00
	OUT PORTC, R16
	LDI R23, 8
	octloop:
		CALL wait
		DEC R23
		BRNE octloop
	
	//Exits interrupt
	LDI R17, 0x02
	OUT EIFR, R17 //Clears INT1 in case
	POP R17
	OUT SREG, R17
	POP R17
	RETI

//LED routines
blink_4th:
	LDI R16, 0b11110111
	OUT PORTC, R16
	CALL wait_1sec
	LDI R18, 1
	RET

blink_3rd:
	LDI R16, 0b11111011
	OUT PORTC, R16
	CALL wait_1sec
	LDI R18, 2
	RET

blink_2nd:
	LDI R16, 0b11111101
	OUT PORTC, R16
	CALL wait_1sec
	LDI R18, 3
	RET

blink_1st:
	LDI R16, 0b11111110
	OUT PORTC, R16
	CALL wait_1sec
	LDI R18, 0
	RET	

//Wait 1 sec - needed because assembly is 8-bit only
wait_1sec:
	LDI R16, 0
	LDI R19, 4 
	quad_loop:
		CALL wait
		INC R16
		CPSE R16, R19 //Loops until register 21 equals 4
		RJMP quad_loop
		RET

wait: // this subroutine creates a delay of 0-255 msec by repeatedly calling delay_1msec
	LDI R22, 250 // choose the number of msec to delay
	count_down:
	CALL delay_1msec
	DEC R22
	BRNE count_down
	RET // return from subroutine

delay_1msec: // this subroutine creates a 1msec delay with TIMER0 register and prescaler of 64 (assumes 16MHz clock)
	LDI R20, 0x00 // clears WGM00 and WGM01 (bits 0 and 1) to ensure Timer/Counter is in normal mode.
	OUT TCCR0A, R20
	
	LDI R20, 0x00 // clear timer register
	OUT TCNT0, R20
	
	LDI R21, 250 // set desired count value for later compare
		
	LDI R20, 0b00000011 //1<<CS01 | 1<<CS00;	TCCR0B = 0x03;  // Start TIMER0, Normal mode, crystal clock, prescaler = 64
	OUT TCCR0B, R20

	keep_counting:
	IN R20, TCNT0  //read TCNT0 and compare to desired count.  Loop until count = 250.
	CPSE R20, R21
	RJMP keep_counting

	LDI R20, 0x00; // Stop TIMER0
	OUT TCCR0B, R20
	RET // return from subroutine
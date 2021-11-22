/*	Author: sspon001
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab 7  Exercise 1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include "../header/io.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum states {init, begin, incr, decr, incrHold, decrHold, reset} state ;
unsigned char pb ;
unsigned char a ;
unsigned short t ;

volatile unsigned char TimerFlag = 0 ;
unsigned long _avr_timer_M = 1 ;
unsigned long _avr_timer_cntcurr = 0 ;
void TimerISR() { 
	TimerFlag = 1 ;
}
void TimerOn() {
	TCCR1B = 0x0B ;
	OCR1A = 125 ;
	TIMSK1 = 0x02 ;
	TCNT1 = 0 ;
	_avr_timer_cntcurr = _avr_timer_M ;
	SREG |= 0x80 ;
}
void TimerOff() {
	TCCR1B = 0x00 ;
}
ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr-- ;
	if (_avr_timer_cntcurr == 0) {
		TimerISR() ;
		_avr_timer_cntcurr = _avr_timer_M ;
	}
}
void TimerSet (unsigned long M) {
	_avr_timer_M = M ;
	_avr_timer_cntcurr = _avr_timer_M ;
}

void Tick() {
	unsigned char a = (~PINA & 0x03) ;
	switch(state) {
		case init:
			break ;
		case begin:
			if (a == 0x03) state = reset ; // 0x03 = 11 = both buttons pressed
			else if ((~PINA & 0x03) == 0x01) state = incr ; // 0x01 = 01 = one button pressed
			else if ((~PINA & 0x03) == 0x02) state = decr ; // 0x02 = 10 = one button pressed
			break ;
		case reset:
			if (a == 0x03) state = reset ; // 0x03 = 11 = both buttons pressed
			else state = begin ;
			break ;
		case incr:	
			if (a == 0x03) state = reset ; // 0x03 = 11 = both buttons pressed
			else if (a == 0x01) state = incrHold ; // if button is held after initial press
			else state = begin ;
                        break ;
		case incrHold:
			if(a == 0x01) state = incrHold ; // if button is held, stay in state
			else if(a == 0x03) state = reset ; // 0x03 = 11 = both buttons pressed
			else state = begin ;
			break ;
		case decr:
			if (a == 0x03) state = reset ; // 0x03 = 11 = both buttons pressed
			else if (a = 0x02) state = decrHold ; // if button is held after initial press
			else state = begin ;
                        break ;
		case decrHold:
			if(a == 0x02) state = decrHold ; // if button is held, stay in state
			else if(a == 0x03) state = reset ; // 0x03 = 11 = both buttons pressed
			else state = begin ;
			break ;
		default:
			state = init ;
			break ;
	}
	switch(state) {
		case init:
			PORTB = 0x00 ;
			state = begin ;
			break ;
		case begin:
			t = 0 ;
			break ;
		case incr:
			if(PORTB < 0x09) PORTB = PORTB + 1 ;
                        break ;
		case decr:
			if(PORTB > 0x00) PORTB = PORTB - 1 ;
                        break ;
		case incrHold:
			if(t == 10){ // 100ms period means 10 ticks of t equals 1 second
				if(PORTB < 0x09) PORTB = PORTB + 1 ;
				t = 0 ;
			}
			else t++ ;
			break ;
		case decrHold:
			if(t == 10){
				if(PORTB > 0) PORTB = PORTB - 1 ;
				t = 0 ;
			}
			else t++ ;
			break ;
		case reset:
			PORTB = 0x00 ;
			break ;
		default:
			PORTB = 0x00 ;
			break ;
	}
	pb = PORTB ; // output to LCD screen
	switch(pb){	
		case 0x00:
			LCD_DisplayString(1,"0") ;
			break ;
		case 0x01:
			LCD_DisplayString(1,"1") ;
			break ;
		case 0x02:
			LCD_DisplayString(1,"2") ;
			break ;
		case 0x03:
			LCD_DisplayString(1,"3") ;
			break ;
		case 0x04:
			LCD_DisplayString(1,"4") ;
			break ;
		case 0x05:
			LCD_DisplayString(1,"5") ;
			break ;
		case 0x06:
			LCD_DisplayString(1,"6") ;
			break ;
		case 0x07:
			LCD_DisplayString(1,"7") ;
			break ;
		case 0x08:
			LCD_DisplayString(1,"8") ;
			break ;
		case 0x09:
			LCD_DisplayString(1,"9") ;
			break ;
		default:
			break ;
	}
}
int main() {
    DDRA = 0x00 ; PORTA = 0xFF ;
    DDRB = 0xFF ; PORTB = 0x00 ;
    DDRC = 0xFF ; PORTC = 0x00 ;
    DDRD = 0xFF ; PORTD = 0x00 ;
    TimerSet(100) ;
    TimerOn() ;
    LCD_init() ;
    while (1) {
	Tick() ;
	while(!TimerFlag) {} ;
	TimerFlag = 0 ;
    }
}

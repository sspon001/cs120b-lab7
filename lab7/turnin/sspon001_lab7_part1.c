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

enum states {start, begin, init, reset, plus, minus, plusOn, minusOn, add, sub} state ;
unsigned char temp = 0x00 ;

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
	if (PORTB == 0x00) LCD_DisplayString(1, "0") ;
	else if (PORTB == 0x01) LCD_DisplayString(1, "1") ;
	else if (PORTB == 0x02) LCD_DisplayString(1, "2") ;
	else if (PORTB == 0x03) LCD_DisplayString(1, "3") ;
	else if (PORTB == 0x04) LCD_DisplayString(1, "4") ;
	else if (PORTB == 0x05) LCD_DisplayString(1, "5") ;
	else if (PORTB == 0x06) LCD_DisplayString(1, "6") ;
	else if (PORTB == 0x07) LCD_DisplayString(1, "7") ;
	else if (PORTB == 0x08) LCD_DisplayString(1, "8") ;
	else if (PORTB == 0x09) LCD_DisplayString(1, "9") ;
	switch(state) {
		case start:
			state = begin ;
			break ;
		case begin:
			state = init ;
			break ;
		case init:
			if ((~PINA & 0x03) == 0x03) state = reset ;
			else if ((~PINA & 0x03) == 0x01) state = add ;
			else if ((~PINA & 0x03) == 0x02) state = sub ;
			break ;
		case reset:
			if ((~PINA & 0x03) == 0x03) state = reset ;
			else state = init ;
			break ;
		case plus:
			if ((~PINA & 0x03) == 0x01) state = plus ;
                        else state = init ;
                        break ;
		case add:
			if (temp >= 0x0A) temp = 0x00 ;
			state = plusOn ;
			break ;
		case sub:
			if (temp >= 0x0A) temp = 0x00 ;
			state = minusOn ;
			break ;
		case plusOn:
			if ((~PINA & 0x03) == 0x01) {
                                ++temp ;
				state = plusOn ;
				if (temp >= 0x0A) state = add ;
                        }
                        else state = plus ;
			break ;
		case minus:
			if ((~PINA & 0x03) == 0x02) state = minus ;
                        else state = init ;
                        break ;
		case minusOn:
			if ((~PINA & 0x03) == 0x02) {
                                state = minusOn ;
				++temp ;
				if (temp >= 0x0A) state = sub ;
                        }
                        else state = minus ;
			break ;
		default:
			state = start ;
			break ;
	}
	switch(state) {
		case start:
			PORTB = 0x07 ;
			break ;
		case begin:
			PORTB = 0x07 ;
			break ;
		case init:
			temp = 0x00 ;
			break ;
		case plusOn:
			break ;
		case minusOn:
			break ;
		case plus:
			break ;
		case minus:
			break ;
		case add:
			if (PORTB < 0x09) PORTB = PORTB + 1 ;
                        break ;
		case sub:
			if (PORTB > 0x00) PORTB = PORTB - 1 ;
                        break ;
		case reset:
			PORTB = 0x00 ;
			break ;
		default:
			PORTB = 0x07 ;
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

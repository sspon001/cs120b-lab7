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
#include "../header/io.h"
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum states {start, init, next, press, rr} state ;
unsigned char alternate = 0x00 ;
unsigned char score = 0x05 ;

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
	if(score == 0x00) LCD_DisplayString(1, "0") ;
	else if(score == 0x01) LCD_DisplayString(1, "1") ;
	else if(score == 0x02) LCD_DisplayString(1, "2") ;
	else if(score == 0x03) LCD_DisplayString(1, "3") ;
	else if(score == 0x04) LCD_DisplayString(1, "4") ;
	else if(score == 0x05) LCD_DisplayString(1, "5") ;
	else if(score == 0x06) LCD_DisplayString(1, "6") ;
	else if(score == 0x07) LCD_DisplayString(1, "7") ;
	else if(score == 0x08) LCD_DisplayString(1, "8") ;
	else if(score == 0x09) {
                LCD_DisplayString(1, "9") ;
		LCD_DisplayString(2, "Victory!") ;
        }

	switch(state) {
		case start:	
				state = init ; 
				break ;
		case init:	
				state = next ; 
				break ;
		case next:	
				if((~PINA & 0x01) == 0x01) state = press ;
				else state = next ;
			       	break ;
		case press:	
				if((~PINA & 0x01) == 0x01) state = press ;
				else state = rr ;
				break ;
		case rr:	
				if((~PINA & 0x01) == 0x01) state = init ;
				else state = rr ;
				break ;
		default:	
				state = start ; 
				break ;
	}
	switch (state) {
		case start:     
				score = 0x05 ;
				break ;
                case init:      
				PORTB = 0x01 ; 
				break ;
		case next:
				if(alternate == 0x00) {
					if(PORTB == 0x04) {
						PORTB = PORTB >> 1 ;
						alternate = 0x01 ;
					}
					else PORTB = PORTB << 1 ;
				}
				else {
					if(PORTB == 0x01) {
              	                        	PORTB = PORTB << 1 ;
                                        	alternate = 0x00 ;
                                	}
                                	else PORTB = PORTB >> 1 ;
				}
				break ;
		case press: 	
				if(PORTB == 0x02) score++ ;
				else {
					if(score > 0) score-- ;
                                }
				state = next ;
				break ;
		case rr:	
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
    TimerSet(300) ;
    TimerOn() ;
    LCD_init() ;
    while (1) {
	Tick() ;
	while(!TimerFlag) {} ;
	TimerFlag = 0 ;
    }
    return 1 ;
}

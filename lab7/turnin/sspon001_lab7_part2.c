/*	Author: sspon001
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab 7  Exercise 2
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

enum BlinkStates {initB, blink} blinkState ;
enum GameStates {initG, idle, press, release, press2} gameState ;
unsigned char pb ;
unsigned char a ;
unsigned char t, temp, score ;

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
	a = (~PINA & 0x01) ;
	switch(blinkState) {
		case initB:
			t = 0 ;
			PORTB = 0x01 ;
			blinkState = blink ;
			break ;
	        case blink: // blink leds in sequence every 300 ms
			switch(t){
				case 0x03:
					PORTB = 0x02 ;
					break ;
				case 0x06:
					PORTB = 0x04 ;
					break ;
				case 0x09:
					PORTB = 0x02 ;
					break ;
				case 0x0C:
					PORTB = 0x01 ;
					t = 0 ;
					break ;
				default:
					break ;
			}
			t++ ;
			break ;
		default:
			break ;
	}
	switch(gameState){
		case initG:
			gameState = idle ;
			score = 0x05 ;
			break ;
		case idle: // wait for user input
			temp = PORTB ;
			if(a == 0x00) gameState = idle ;
			else if(a == 0x01) gameState = press ;
			break ;
		case press: // user input, if pressed when PORTB = 0x02, then score incr, otherwise score decr
			PORTB = temp ;
			if(a == 0x01) gameState = press ;
			else if(a == 0x00) gameState = release ;
			if(PORTB == 0x02){
				if(score < 9) score++ ;
			}
			else{
				if(score > 0) score-- ;
			}
			t-- ; // pauses the current LED sequence
			break ;
		case release: // on release of button, if score is 9 display victory screen, otherwise return to game
			PORTB = temp ;
			if(a == 0x00) gameState = release ;
			else if(a == 0x01){
				if(score < 9) gameState = idle ;
				else gameState = press2 ;
			}
			t-- ;// pauses the current LED sequence
			break ;
		case press2: // when released, restart game
			PORTB = temp ;	
			t-- ;
			if(a == 0x01) gameState = press2 ;
			else if(a == 0x00){
				gameState = initG ;
				t = 0 ;
				PORTB = 0x01 ;
			}	
			break ;
		default:
			break ;
	}
	switch(score){	// output current score to LCD
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
			LCD_DisplayString(1,"Victory!") ;
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
    blinkState = initB ;
    gameState = initG ;
    while (1) {
	Tick() ;
	while(!TimerFlag) {} ;
	TimerFlag = 0 ;
    }
}

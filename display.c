/*
 * File:   display.c
 * Author: osama
 *
 * Created on March 31, 2024, 3:59 PM
 */


#include <avr/io.h>
#include "display.h"
#include <util/delay.h>



void displayNumber( const uint8_t *digits, int mode){ 
    PORTD.DIR |= 0x1f;
    for(int i = 3; i >= 0 ; i--){
        switch(digits[i]){
            case 0:
                PORTD.OUT |= ALLOFF;
                PORTA.OUT = N0;
                break;
            case 1:
                PORTD.OUT |= ALLOFF;
                PORTA.OUT = N1;
                break;
            case 2:
                PORTD.OUT |= ALLOFF;
                PORTA.OUT = N2;
                break;
            case 3:
                PORTD.OUT |= ALLOFF;
                PORTA.OUT = N3;
                break;
            case 4:
                PORTD.OUT |= ALLOFF;
                PORTA.OUT = N4;
                break;
            case 5:
                PORTD.OUT |= ALLOFF;
                PORTA.OUT = N5;
                break;
            case 6:
                PORTD.OUT |= ALLOFF;
                PORTA.OUT = N6;
                break;
            case 7:
                PORTD.OUT |= ALLOFF;
                PORTA.OUT = N7;
                break;
            case 8:
                PORTD.OUT |= ALLOFF;
                PORTA.OUT = N8;
                break;
            case 9:
                PORTD.OUT |= ALLOFF;
                PORTA.OUT = N9;
                break;
            default:
                PORTD.OUT |= ALLOFF;
                PORTA.OUT = CLEAR;
            }
    
        if(mode == 1){
            switch(i){
                case 0:
                    PORTD.OUT &= D1;
                    _delay_us(400);
                    break;
                case 1:
                    PORTD.OUT &= D2;
                    _delay_us(400);
                    break;
                case 2:
                    PORTD.OUT &= D3;
                    PORTA.OUT = DP(PORTA.OUT);
                    _delay_us(400);
                    break;
                case 3:
                    PORTD.OUT &= D4;
                    _delay_us(400);
                    break;
            }
        }
        else{
            switch(i){
                case 1:
                    PORTD.OUT &= D1;
                    _delay_us(400);
                    break;
                case 2:
                    PORTD.OUT &= D2;
                    PORTA.OUT = DP(PORTA.OUT);
                    _delay_us(400);
                    break;
                case 3:
                    PORTD.OUT &= D3;
                    _delay_us(400);
                    break;
                case 0:
                    PORTD.OUT &= D4;
                    _delay_us(400);
                    break;
            }
        }                 
    }
}

void displayInit(){
    PORTA.DIR = 0xff;
    PORTD.DIR = 0x1f;
    PORTD.OUT |= 0x1f;
}

void updDisAr(uint8_t *digits, int number){
    for(int i = 3; i>=0; i--){
                digits[i] = number % 10;
                number /= 10;
            }
}
    

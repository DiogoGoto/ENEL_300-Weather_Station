/*
 * File:   display.h
 * Author: diogo
 *
 * Created on 18 de Fevereiro de 2024, 14:54
 */
//Assuming PORT A -> a-g on display
#ifndef DISPLAY_H
#define DISPLAY_H
#include <avr/io.h>


#define CLEAR 0x00 //LOW to letter pins
#define N0 0x3f
#define N1 0x06
#define N2 0x5b 
#define N3 0x4f
#define N4 0x66
#define N5 0x6d
#define N6 0x7d
#define N7 0x07
#define N8 0x7f
#define N9 0x6f

#define DP(N) (N|0x80)

#define CHAR_C 0x39
#define CHAR_H 0x76 

#define ALLOFF (0x1f) // HIGH to all Digit PIn
#define D1 ~(0x02)
#define D2 ~(0x04)
#define D3 ~(0x08)
#define D4 ~(0x10)

void displayNumber(const uint8_t *digits,  int mode);
/*
 * digits is a list of len 4 that contain the numbers we want to display
 * mode indicate the display mode, 1 for temp, 0 for humidity  
 */

void displayInit();
/*
 * Sets PortA as output and the four first pins of PortD as OUTPUT 
 * set PortD to HIGH to turn off all digits
 */

void updDisAr(uint8_t *digits, int number);
/*
 * digits is the list of len 4 that will be used to the save each individual
 * digit that makes number
 */
#endif
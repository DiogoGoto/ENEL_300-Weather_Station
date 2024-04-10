/*
 * File:   Wind_speed.c
 * Author: diogo
 *
 * Created on 7 de Março de 2024, 14:02
 */


#include <xc.h>
#include <avr/io.h>
#include "display.h"
#include "spi_f.c"

#define PERIOD 1024
#define WIND_SENSOR 0x6 // on PD5
#define BUTTOM 0x5// PD5
#define COOLDOWN  333333//Buttom Cooldown // time it couts up to before timing out
#define TIMEOUT 200

int main(void) {
    
    // -- RTC Setup --
    
    SREG = 0b10000000; //Enables interrupt  
    
    RTC.CLKSEL = 0x01; // Bits[0-1] = Select RTC Clock = 32.768/32 CLK
    
    RTC.CMP = PERIOD; //1s in 32.768Hz/32 //this step might be optional TEST LATER
    RTC.PER = PERIOD; // Max value it counts to //this step might be optional TEST LATER
    
    RTC.INTCTRL = 0x01; // Bit 1 - cmp interrupt, Bit 0 - Overflow int
    
    RTC.CTRLA = 0x01; // Bits[3-6] = Prescaler = CLK/1 // Bit 0 RTC Enable
    
    RTC.PITINTCTRL = 0x01; //Enables Periodic Interrupts// Bit 0 EN
    
    RTC.PITCTRLA = 0x09; //0b0 0001 00 1 // Bits[3-6] = # Clock cycles = 4 // bit 0 = PIT Enable
    
    // --AC Initialization --
    
    AC0.CTRLA = 0b00000111; // 0 0 X 00 11 1  bits [2:1] is the hysteris, bit 0 is enable
    AC0.MUXCTRL = 0b00011011;// 0 0 011 011 // sets pin PD6 as + side of the comparator and - to DAC REF
    VREF.ACREF = 0b00000000; // Sets the REF voltage to 1.024V
    AC0.DACREF = 0xFF;  // - terminal voltage = 255/256 * REF Voltage (~1.024v)
    AC0.INTCTRL = 0x21; // Enables interrupts
                        // Interrupts happens on rising edges ( + goes above -)
    
#if 0
    // TCA Initiazliation
    
    TCA0.SINGLE.CTRLA = 0x0f;
    TCA0.SINGLE.CMP0 = 33;
    TCA0.SINGLE.PER = 33;
#endif
    
    
    //-- Port Initialization --
    

    PORTD.DIRCLR |= (1 << WIND_SENSOR);
    PORTD.OUT = 0x0f;
    
    
    SPI_init(1); // SPI - Initialization Master
    
    
    // --  Variables --
    
    uint8_t counter = 0;
    unsigned int counter_CLD = COOLDOWN;
    uint8_t flags = 0x00; 
    
    uint8_t digits[4] = {0,0,0,0};
    
    uint8_t windSpeed = 0;
    
    uint8_t received_data = 0;
    PORTA.DIRCLR = 0x01; // Set PA 1 as input
    
    while(1){
        
        // Wait for a second to register the wind speed
        if(RTC.INTFLAGS & 0x01){
            RTC.INTFLAGS = 0x01; // clear the flag 
            windSpeed = counter; //1-1 ratio between windspeed (KPH) to Hz
            counter = 0 ; // reset the rotation conuter
            
            SPI_transmit(windSpeed, &received_data);
            
        }
        
        if(AC0.STATUS & 0x01){ //reads a one every half rotation
                counter++;
                AC0.STATUS |=0x01;
            }

          
    }

    return 0;
}

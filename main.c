/*
 * File:   main.c
 * Author: diogo
 *
 * Created on 17 de Fevereiro de 2024, 14:56
 */

#include <xc.h>
#include <avr/io.h>
#include <util/delay.h>
#include "display.c" 
#include "DHT22.c"
#include "spi_f.c"


#define F_CPU 4000000 // 4MHz
#define SENSOR 0x7 //port number

#define BUTTOM 0x6 //port number
#define COOLDOWN 1000 //Buttom Cooldown


int main(void) {
    
    SREG = 0b10000000; //Enables interrupt  
    
    // -- RTC Setup -- 
    RTC.CLKSEL = 0x01; // Bits[0-1] = Select RTC Clock = 32.768/32 CLK
    RTC.CMP = 0x1400; //0001 0100 0000 0000 // 5120 = 5s in 32.768Hz/32 //this step might be optional TEST LATER
    RTC.PER = 0x1400; // Max value it counts to //this step might be optional TEST LATER
    RTC.INTCTRL = 0x01; // Bit 1 - cmp interrupt, Bit 0 - Overflow int
    RTC.CTRLA = 0x01; // Bits[3-6] = Prescaler = CLK/1 // Bit 0 RTC Enable
        
    // -- Display --
    uint8_t digits[4] = {0, 0, 0, 0};
    displayInit();
    
    
    // -- SPI Setup --
    SPI_init(0); // Initializes as slave
    
    // -- Port- Setup--
    
    
    PORTD.DIRCLR |= (1<<BUTTOM);
    PORTD.DIR |= 0x20;    
    
    // -- DHT Setup
    DHT sensor;
    DHT_init(&sensor, SENSOR);
    
    // -- Variables Initialization --
    
    uint8_t windSpeed = 0;
    uint8_t mode = 0; 
    /*
     * 0 - Humidity
     * 1 - Temperature
     * 2 - Wind Speed
     */
    
        
    unsigned int counter_CLD = 0 ;
    
    
     
    _delay_ms(TIME(1000)); // wait 1 sec for Sensor to stabilize
    
    
    while(1){
        
        // -- Temperature/Humidity --
        
        if(RTC.INTFLAGS & 0x01){ // Check if the counter reached 5secs
            RTC.INTFLAGS = 0x01; // writes an 1 to the overflow flag to clear it
            DHT_readData(&sensor);
        }
        
        // -- Wind Speed --
        if(!SPI_transmit(1,&windSpeed)) // Tries to read data from SPI
            windSpeed = 0; // Clears the data if fails to read
        
        
        // -- Display -- 
        
        switch(mode){
            case 0:
                updDisAr(digits, sensor.humidity);
                break;
            case 1:
                updDisAr(digits, sensor.temperature);
                break;
            case 2:
                updDisAr(digits, windSpeed);
                break;
            default:
                mode = 0;
        }
        displayNumber(digits, mode);
        
        if(!(PORTD.IN & (1 << BUTTOM))){
            while(!(PORTD.IN & (1 << BUTTOM))){}
            if(counter_CLD == 0){
                mode += 1;
                counter_CLD = COOLDOWN;
            }
        }
        
        if(counter_CLD != 0)
            counter_CLD--;
    }      
    return 0;
}

/*
 * File:   main.c
 * Author: diogo
 *
 * Created on 17 de Fevereiro de 2024, 14:56
 */

#include <xc.h>
#include <avr/io.h>
#include <util/delay.h>
#include<avr/interrupt.h> // not used can remove
#include "display.c" // Contain hex code for numbers in 7 segs display



#define SENSOR 0x7 //port number
#define TIMEOUT 200 // time it couts up to before timing out


#define BUTTOM 0x6 //port number
#define COOLDOWN 17000 //Buttom Cooldown


#define TIME(T) (T<<2) //library for delays is set for a different clock speed
                       //Might need to adjust this (just test with oscilloscope )

void main(void) {
    
    SREG = 0b10000000; //Enables interrupt  
    
    //RTC Setup
    RTC.CLKSEL = 0x01; // Bits[0-1] = Select RTC Clock = 32.768/32 CLK
    
    RTC.CMP = 0x1400; //0001 0100 0000 0000 // 5120 = 5s in 32.768Hz/32 //this step might be optional TEST LATER
    RTC.PER = 0x1400; // Max value it counts to //this step might be optional TEST LATER
    
    RTC.INTCTRL = 0x01; // Bit 1 - cmp interrupt, Bit 0 - Overflow int
    
    RTC.CTRLA = 0x01; // Bits[3-6] = Prescaler = CLK/1 // Bit 0 RTC Enable
    
    RTC.PITINTCTRL = 0x01; //Enables Periodic Interrupts// Bit 0 EN
    
    RTC.PITCTRLA = 0x09; //0b0 0001 00 1 // Bits[3-6] = # Clock cycles = 4 // bit 0 = PIT Enable
    
    
    //Initial Conditions
    PORTD.DIR |= 0x0f;
    PORTD.OUT = 0xff;
    
    PORTD.DIRCLR |= (1<<BUTTOM);
    
    PORTD.DIR |= 0x20;
    PORTA.DIR = 0xff;
    PORTA.OUT = 0xff;
    
    //Variables
    uint8_t flags = 0x00; 
    //bit 3 - UNUSED
    //bit 2 - UNUSED
    //bit 1 - 1 = Disp Temp, 0 = Disp humi
    //bit 0 - Received response from Sensor
    
        
    unsigned int counter_CLD = 0 ;
    uint16_t timeout_counter = 0;
    uint8_t result  = 0;
    uint8_t rise = 0;
    uint8_t fall = 0;
    
    uint8_t bits[5] = {0,0,0,0,0};
    
    uint16_t temperature = 0; 
    uint16_t humidity = 0;
    
    uint8_t digits[8] = {0,0,0,0,0,0,0,0};
   

    
    _delay_ms(TIME(1000)); // wait 1 sec for Sensor to stabilize
    
    
    while(1){
        
        if(RTC.INTFLAGS & 0x01){ // Check if the counter reached 5secs
            RTC.INTFLAGS = 0x01; // writes an 1 to the overflow flag to clear it
            PORTD.DIR |= (1 << SENSOR); // set PD2 as OUTPUT
            PORTD.OUT &= ~(1 << SENSOR); // Start Trigger (Pull LOW)
            _delay_ms(2); // 500us trigger signal (LOW) TEST with macro
            PORTD.OUT |= (1 << SENSOR); //Ends trigger(Pull HIGH)
            PORTD.DIRCLR |= (1 << SENSOR); //set PD2 as INPUT
        
            _delay_us(TIME(30)); // time btw trigger and response
        
        
            if(!(PORTD.IN & (1 << SENSOR))){
                PORTD.DIR |= 0x20; //For Debugging 
                _delay_us(TIME(80)); // time btw response and preparing for first bit
  
                flags |= 0x01; //set the flag the sensor did not timeout
                timeout_counter = 0;
                while(PORTD.IN & (1 << SENSOR)){
                    timeout_counter++;
                    if(timeout_counter > TIMEOUT){
                        flags &= ~0x01; 
                        break;
                    }
                }
                
                
                if(flags & 0x01){ //if(sensor started sending the first bit)
                    //Reset the variable so that the previous bits does not corrupt the data
                    temperature = 0; 
                    humidity = 0;
                    result = 0;
                    
                    for(int i = 0 ; i < 16; i++){ //Reads Humidity
                        while(!(PORTD.IN & (1 << SENSOR))) {}//wait for Sensor HIGH <=> Next bit
                        
                        if(PORTD.IN & (1 << SENSOR)){ //if(SENSOR pulls high) (start of the bit)
                            rise = 0;
                            fall = 0;
                            while(PORTD.IN & (1 << SENSOR)){// Count how long it takes to pull LOW (end of the bit)
                                fall++;
                            }
                        }

                        if( ((fall - rise) > 11) && (fall != rise)){ // 11 cycles ~< 70us = 1 (logic) //might need to change here (Clock Depedant)
                            humidity |= 1<<(15-i); //bitwise and to register the one
                            //PORTD.OUT |= 0x20; //For Debugging
                        }
                        //PORTD.OUT &= ~0x20; //For Debugging
                    }//for loop

                    for(int i = 0 ; i < 16; i++){//Reads Temperature
                        while(!(PORTD.IN & (1 << SENSOR))) {}//wait for Sensor HIGH <=> Next bit
                        
                        if(PORTD.IN & (1 << SENSOR)){ //if(SENSOR pulls high) (start of the bit)
                            rise = 0;
                            fall = 0;
                            while(PORTD.IN & (1 << SENSOR)){// Count how long it takes to pull LOW (end of the bit)
                                fall++;
                            }
                        }

                        if( ((fall - rise) > 11) && (fall != rise)){// 11 cycles ~< 70us = 1 (logic) //might need to change here (Clock Depedant)
                            temperature |= 1<<(15-i); //bitwise and to register the one
                            //PORTD.OUT |= 0x20; //For Debugging
                        }
                        //PORTD.OUT &= ~0x20; //For Debugging
                    }//for loop

                    for(int i = 0; i < 8 ; i++){
                        while(!(PORTD.IN & (1 << SENSOR))) {} //wait for Sensor HIGH <=> Next bit
                        
                        //PORTD.OUT |= 0x20; //For Debugging
                        if(PORTD.IN & (1 << SENSOR)){ //if(SENSOR pulls high) (start of the bit)
                            rise = 0;
                            fall = 0;
                            while(PORTD.IN & (1 << SENSOR)){ // Count how long it takes to pull LOW (end of the bit)
                                fall++;
                            }
                        }

                        if( ((fall - rise) > 11) && (fall != rise)){ // 11 cycles ~< 70us = 1 (logic) //might need to change here (Clock Depedant)
                            result |= 1<<(7-i); //bitwise and to register the one
                            //PORTD.OUT |= 0x20; //For Debugging
                        }
                        //PORTD.OUT &= ~0x20; //For Debugging
                    }//for loop
                }//if(prep for first bit)
                
                bits[0] = (uint8_t) (humidity >> 8);
                bits[1] = (uint8_t) humidity;
                bits[2] = (uint8_t) (temperature >> 8);
                bits[3] = (uint8_t) temperature;

                bits[4] = result;


                if((uint8_t)(bits[0] + bits[1] + bits[2] + bits[3]) == bits[4]){ // the sum of the 4 first bytes should be equal to the last byte sent by the sensor
                    for(int i = 7; i >= 4 ; i--){//gets the digits of the temperature value
                        digits[i] = temperature % 10;// the modulo 10 will return the single digit
                        temperature /= 10; //as temperature is an integer the division by 10 will get rid of the decimal part
                    }
                    for(int i = 3; i >= 0 ; i--){//gets the digits of the humidity value
                        digits[i] = humidity % 10;
                        humidity /= 10;
                    }
                }//if(data is correct)
                else
                    for(int i = 0; i < 8; i++){// if sensor fails to send data, set everything to 0
                        digits[i] = 0;
                    } 
                }//if (sensor responds)
        }//if(counter reached 5s)
        
        
        /*
         The number for the reading you can find by making any of the port switch states inside the while loop
         and count how many time it switch using the oscilloscope, the short pulse are 0's and longer are 1's 
         any number higher # of switch in a 0 and lower than  switches in a 1 should work
         
         */
        
        // Split the digit list
        uint8_t temperature[4]= {0,0,0,0};
        uint8_t humidity[4]= {0,0,0,0};
        for (int i = 0; i < 4; ++i) {
            temperature[i] = digits[i];         
            humidity[i] = digits[i + 4];    
        }
        // -- Display -- 
        if(flags & 0x02){
            displayNumber(temperature, 1);  
        }//if mode
        else{
            displayNumber(humidity,0);
        }
        
        if(!(PORTD.IN & (1 << BUTTOM))){
            while(!(PORTD.IN & (1 << BUTTOM))){}
            if(counter_CLD == 0){
                flags ^= 0b00000010;
                counter_CLD = COOLDOWN;
            }
        }
        
        if(counter_CLD != 0)
            counter_CLD--;
    }      
    return;
}

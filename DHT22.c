/*
 * File:   DHT22.c
 * Author: diogo
 *
 * Created on 6 de Abril de 2024, 17:56
 */


#include <xc.h>
#include <avr/io.h>
#include "DHT22.h"


void DHT_init(DHT *sensor, uint8_t sensor_pin){
    sensor->port = sensor_pin;
    sensor->temperature = 0;
    sensor->humidity = 0;
}

uint8_t DHT_readData(DHT *sensor){
    // -- Variables --
    uint8_t rise = 0, fall = 0, flags = 0, bits[5];
    long unsigned int timeout_counter = 0;
    
    PORTD.DIR |= (1 << sensor->port); // set PD2 as OUTPUT
    PORTD.OUT &= ~(1 << sensor->port); // Start Trigger (Pull LOW)
    _delay_ms(2); // 500us trigger signal (LOW) TEST with macro
    PORTD.OUT |= (1 << sensor->port); //Ends trigger(Pull HIGH)
    PORTD.DIRCLR |= (1 << sensor->port); //set PD2 as INPUT
    
    _delay_us(TIME(30)); // time btw trigger and response
        
        
    if(!(PORTD.IN & (1 << sensor->port))){
        PORTD.DIR |= 0x20; //For Debugging 
        _delay_us(TIME(80)); // time btw response and preparing for first bit
        
        flags |= 0x01; //set the flag the sensor did not timeout
        timeout_counter = 0;
        while(PORTD.IN & (1 << sensor->port)){
            timeout_counter++;
            if(timeout_counter > TIMEOUT){
                flags &= ~0x01; 
                break;
            }   
        }
                
                
                if(flags & 0x01){ //if(sensor started sending the first bit)
                    //Reset the variable so that the previous bits does not corrupt the data
                    sensor->temperature = 0; 
                    sensor->humidity = 0;
                    sensor->check= 0;
                    
                    for(int i = 0 ; i < 16; i++){ //Reads Humidity
                        while(!(PORTD.IN & (1 << sensor->port))) {}//wait for Sensor HIGH <=> Next bit
                        
                        if(PORTD.IN & (1 << sensor->port)){ //if(SENSOR pulls high) (start of the bit)
                            rise = 0;
                            fall = 0;
                            while(PORTD.IN & (1 << sensor->port)){// Count how long it takes to pull LOW (end of the bit)
                                fall++;
                            }
                        }

                        if( ((fall - rise) > 11) && (fall != rise)){ // 11 cycles ~< 70us = 1 (logic) //might need to change here (Clock Depedant)
                            sensor->humidity |= 1<<(15-i); //bitwise and to register the one
                            //PORTD.OUT |= 0x20; //For Debugging
                        }
                        //PORTD.OUT &= ~0x20; //For Debugging
                    }//for loop

                    for(int i = 0 ; i < 16; i++){//Reads Temperature
                        while(!(PORTD.IN & (1 << sensor->port))) {}//wait for Sensor HIGH <=> Next bit
                        
                        if(PORTD.IN & (1 << sensor->port)){ //if(SENSOR pulls high) (start of the bit)
                            rise = 0;
                            fall = 0;
                            while(PORTD.IN & (1 << sensor->port)){// Count how long it takes to pull LOW (end of the bit)
                                fall++;
                            }
                        }

                        if( ((fall - rise) > 11) && (fall != rise)){// 11 cycles ~< 70us = 1 (logic) //might need to change here (Clock Depedant)
                            sensor->temperature |= 1<<(15-i); //bitwise and to register the one
                            //PORTD.OUT |= 0x20; //For Debugging
                        }
                        //PORTD.OUT &= ~0x20; //For Debugging
                    }//for loop

                    for(int i = 0; i < 8 ; i++){
                        while(!(PORTD.IN & (1 << sensor->port))) {} //wait for Sensor HIGH <=> Next bit
                        
                        //PORTD.OUT |= 0x20; //For Debugging
                        if(PORTD.IN & (1 << sensor->port)){ //if(SENSOR pulls high) (start of the bit)
                            rise = 0;
                            fall = 0;
                            while(PORTD.IN & (1 << sensor->port)){ // Count how long it takes to pull LOW (end of the bit)
                                fall++;
                            }
                        }

                        if( ((fall - rise) > 11) && (fall != rise)){ // 11 cycles ~< 70us = 1 (logic) //might need to change here (Clock Depedant)
                            sensor->check |= 1<<(7-i); //bitwise and to register the one
                            //PORTD.OUT |= 0x20; //For Debugging
                        }
                        //PORTD.OUT &= ~0x20; //For Debugging
                    }//for loop
                }//if(prep for first bit)
                
                bits[0] = (uint8_t) (sensor->humidity >> 8);
                bits[1] = (uint8_t) sensor->humidity;
                bits[2] = (uint8_t) (sensor->temperature >> 8);
                bits[3] = (uint8_t) sensor->temperature;

                bits[4] = sensor->check;


                if(!((uint8_t)(bits[0] + bits[1] + bits[2] + bits[3]) == bits[4])){ // the sum of the 4 first bytes should be equal to the last byte sent by the sensor
                    sensor->humidity = 0;
                    sensor->temperature = 0;
                    return 0;
                }//if(data is wrong)
                return 1;
                }//if (sensor responds)
    return 0;
}



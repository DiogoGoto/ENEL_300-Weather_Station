/*
 * File:   DHT22.h
 * Author: diogo
 *
 * Created on 6 de Abril de 2024, 18:00
 */


#ifndef DHT_H
#define DHT_H

#include <xc.h>
#include <avr/io.h>


#define TIMEOUT 200 // time it couts up to before timing out
#define TIME(T) (T<<2) //library for delays is set for a different clock speed
                       //Might need to adjust this (just test with oscilloscope )

typedef struct DHT22{
    uint8_t port;
    uint16_t temperature;
    uint16_t humidity;
    uint8_t check;
}DHT;

void DHT_init(DHT *sensor, uint8_t sensor_pin);
/*
 * Initializes the DHT_sensor with the port 
 * and zero out temperature, humidity and check
 */

uint8_t DHT_readData(DHT *sensor);
/*
 * read the data from the sensor and saves on the structure
 * return a 1 on a sucesfull reading
 * returns a 0 on a failed reading and clears the data collected
 */

#endif
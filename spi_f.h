/*
 * File:   SPI_f.h
 * Author: diogo
 *
 * Created on 5 de Abril de 2024, 08:20
 */

#ifndef SPI_F_H
#define SPI_F_H
#include <avr/io.h>

void SPI_init(uint8_t master); 
/*
* Initializes SPI 1 as
 * master, master = 1
 * slave , master = 0
 * 
 * SPI 1 Ports | MASTER | Slave
 * MOSI PC0    |  OUT   |  IN
 * MISO PC1    |  IN    |  OUT
 * SCK  PC2    |  OUT   |  IN
 * SS   PC3    |  OUT   |  IN
 * 
 * Note: As master don't set SS as input never in the code          
*/

uint8_t SPI_transmit(uint8_t data_send, uint8_t *data_received);
/*
 * Sends data to the other device and save its response
 * Returns a 1 if successfully send
 * Return a 0 if  failed (SPI is busy)
 */
#endif
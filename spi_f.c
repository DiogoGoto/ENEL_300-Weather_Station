/*
 * File:   SPI_f.c
 * Author: diogo
 *
 * Created on 5 de Abril de 2024, 08:19
 */

#include "spi_f.h"
#include <xc.h>
#include <avr/io.h>
#include <util/delay.h>



void SPI_init(uint8_t master){
    if(master){
        PORTC.DIRSET = 0b1101;  //Set the SS, MOSI, SCK ports as OUTPUT
        PORTC.OUT = 0X8; //Set SS as HIGH

        SPI1.CTRLB = 0b00000001; 
        /*
         *            0x7 - Buffer Enable
         *            0x6 - Buffer Wait Receive Enable
         *            0x2 - Slave Select Disable (default)
         *            0x02 : 0x01 - Mode 1 by testing
        */
        SPI1.CTRLA = 0b00100001;   
        /*
         *                      DORD - MSB first (default)
         *                      MASTER - 1 set this device as master
         *                      CLK2X - disable 2x the CLK frequency (default)
         *                      PRE ESC - 0x0 = CLK_per/4 (1MHz) (default)
         *                      SPI_Enable - 1 Enables SPI
         */

        SPI1.INTCTRL = 0b00000001; // Enables Interrupts for SPI

        SPI1.DATA = 0xff; //Write dummy Data to initialize the register
        _delay_us(100);
    }
    else{
        PORTC.DIR = 0b0010; // Set MISO as OUTPUT
        SPI1.CTRLB = 0b11000000; 
        /*
         *            0x7 - Buffer Enable
         *            0x6 - Buffer Wait Receive Enable
         *            0x2 - Slave Select Disable (for MASTER)
         *            0x02 : 0x01 - Mode 0 by default
         */
        SPI1.CTRLA = 0b00000001;   
        /*
         *            DORD - MSB first (default)
         *            MASTER - 0 set this device as slave (Default)
         *            CLK2X - disable 2x the CLK frequency (default)
         *            PRE ESC - 0x0 = CLK_per/4 (1MHz) (default)
         *            SPI_Enable - 1 Enables SPI
         */

        SPI1.INTCTRL = 0b11000001;
        /*
         *            RXCIE - Receive Complete Enable
         *            TXCIE - Transfer Complete Enable
         *            IE - Interrupt Enable
         */
        SPI1.DATA = 0x23;
    }
}

uint8_t SPI_transmit(uint8_t data_send, uint8_t *data_received){
    if(SPI1.INTFLAGS & 0x80){ // Checks for completed send/receive operation
                                  //Reading a 1 Clears the Flag
        PORTC.OUT &= ~0x8; // set SS to LOW starting communication
        SPI1.DATA = data_send; //Write the Data to be sent to the register
        _delay_us(30); //Wait for Transmit to be completed
        
        SPI1.DATA = 0; //send dummy data so that the device can send the data from the firt request
        _delay_us(30) ;
        *data_received = SPI1.DATA; // Saves whatever data was sent by the Slave
        PORTC.OUT |= 0x8; // set SS to HIGH ending communication
        //_delay_us(500); //TEST W/O
        return 1;
      }
    else
        return 0;
}
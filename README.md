# ENEL 300 Weather Station
-------------------------------------------------------------------------------------------------
## Description
This is was a group project done for a class (ENEL 300, UCalgary), in this project we developed a weather station to collect and display data. In this project we are using 2 AVR128DB28 communicating through SPI.
The AVR with the display is the secondary Device and the one with the wind_speed is the primary

#### Members:
 - Andrew Hartfelder
 - Diogo Goto
 - Nicholas Watmough
 - Osama Al Riyami
 - Toluwalase Oseni


-------------------------------------------------------------------------------------------------
## Functionalities
- Reads Temperature
- Reads Humidity
- Reads Wind Speed
-------------------------------------------------------------------------------------------------
## Details
#### Display
 - H - Humidity (%)
 - C - Temperature (C)
 - U - Wind Speed (Km/h)
 
#### Temperature/Humidity
The temperature and humidity sensor is measured using a DHT-22, a digital sensor, and the code wrote for it was made considering a clock frequency of 4 MHzfor different frequency modifications have to made to the part it determines if it read a 1 or a 0 more details on DHT.c file.

#### Wind Speed
The design of the anemometer was made such that the 2x RPS is equivalent to the Wind Speed in Km/h. The equation that describes RPS in Km\h is:

                                                            Rotations_in_1S * 2 pi * R * 3.6 

So by having a radius ~4.4 cm the conversion would be almost 1-1. The way our sensor measures wind speed is by shinning a LED thought a hole in the axis of the anemometer, and having a photo diode on the other side to register the LED shinning, keep in mind the LED will shine twice (once per 180 degrees) so to take that into account our anemometer has a radius of ~8.8.

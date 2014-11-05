#!/usr/bin/python2.7

#-------------------------------------------------
# nRF24L01+ test program for raspberry pi (rev 2, Debian)
# Uses spidev and pigpio libraries for spi and gpio, resp.
#-------------------------------------------------

import pigpio
import spidev
import time
import atexit

class nrf24lib:
    
    #spi = None
    
    def __init__(self):
        print "Welcome to the nRF24L01+ Python library on SensorNet."
        self.spi = spidev.SpiDev()
        self.spi.open(0,0)

    def sendByte(self):
        print "Executing SPI send byte"
        self.spi.xfer([0x5A])

    def close(self):
        print "Closing..."
        self.spi.close()
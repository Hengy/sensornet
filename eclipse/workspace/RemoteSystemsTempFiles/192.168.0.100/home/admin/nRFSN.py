#!/usr/bin/python2.7

#-------------------------------------------------
# nRF24L01+ test program for raspberry pi (rev 2, Debian)
# Uses spidev and pigpio libraries for spi and gpio, resp.
#-------------------------------------------------

import pigpio
import spidev
import time
import atexit

class nRFSNlib:
    
    def __init__(self, CEpin, IRQpin, CSNpin):
        print "Welcome to the nRF24L01+ Python library on SensorNet."
        
        self.CEpin = CEpin
        self.IRQpin = IRQpin
        if CSNpin == 8:
            self.CSNpin = 8
        elif CSNpin == 7:
            self.CSNpin = 8
        else:
            self.CSNpin = CSNpin
        
        self.spi = spidev.SpiDev()
        self.spi.open(0,0)

    def sendByte(self):
        print "Executing SPI send byte"
        self.spi.xfer2([0x5A,0x5A,0x5A,0x5A,0x5A])

    def close(self):
        print "Closing..."
        self.spi.close()
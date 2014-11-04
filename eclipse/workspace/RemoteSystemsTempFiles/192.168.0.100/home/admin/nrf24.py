#!/usr/bin/python2.7

import pigpio
import time

SPI_MOSI = 10
SPI_MISO = 9
SPI_SCLK = 11
SPI_CSN = 7

gpio = pigpio.pi()

gpio.set_mode(SPI_MOSI, pigpio.OUTPUT)
gpio.set_mode(SPI_MISO, pigpio.OUTPUT)
gpio.set_mode(SPI_SCLK, pigpio.OUTPUT)
gpio.set_mode(SPI_CSN, pigpio.OUTPUT)

spih = gpio.spi_open(0, 4000000, 0)

gpio.write(25, 1)
time.sleep(0.005)
gpio.write(25, 0)

i = 0
for i in range(0,5):
    (count, rx_data) = gpio.spi_xfer(spih, b'\x55')
    print count
    print rx_data[0]

gpio.write(25, 1)
print "Done"
gpio.spi_close(spih)
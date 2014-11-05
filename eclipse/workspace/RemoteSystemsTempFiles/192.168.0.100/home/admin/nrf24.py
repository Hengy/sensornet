#!/usr/bin/python2.7

import pigpio
import spidev
import time

print "Starting SPI test..."

gpio = pigpio.pi()

spi = spidev.SpiDev()
spi.open(0,0)
spi.max_speed_hz = 3000000
spi.mode = 0

gpio.set_mode(25, pigpio.OUTPUT)

gpio.write(25, 0)

for i in range(0,5):
    spi.xfer([0x55])

gpio.write(25, 1)
print "Done SPI test."
spi.close()

print "Starting callback test..."

gpio.set_mode(24, pigpio.INPUT)

global done
done = False

def cbf(pin, level, tick):
    print "Pin: ", pin
    print "Level: ", level
    print "Tick: ", tick
    global done
    done = True
    return

cb1 = gpio.callback(24, pigpio.RISING_EDGE, cbf)

while not done:
    pass

print "Done callback test."
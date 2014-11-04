#!/usr/bin/python2.7

import RPi.GPIO as GPIO
import spidev
import time

spi = spidev.SpiDev()
spi.open(0,0)


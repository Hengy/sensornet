#!/usr/bin/python2.7

import pigpio
import spidev
import time
import SensorNetLog
import nRFSNlib
import random

print "Starting"

spi = spidev.SpiDev()
spi.open(0,0)
spi.max_speed_hz=3000000

gpio = pigpio.pi()

gpio.set_mode(4, pigpio.INPUT)

time.sleep(0.1)

log = SensorNetLog.SensorNetLog()
nRFSN = nRFSNlib.nRFSNlib(25, 4, 8, spi, gpio, log)

nRFSN.setTXMode()

while True:
    word = raw_input('Enter your word: ')
    if word == 'exit':
        break
    print word
    for i in range(len(word.strip())):
        nRFSN.BufOut[i] = ord(word[i])
    nRFSN.transmit(len(word.strip()))

    while nRFSN.Status == 0x1E:
        nRFSN.clearInt(nRFSN.TX_DS)
        time.sleep(0.05)
        nRFSN.updateStatus()

    #gpio.wait_for_edge(4,pigpio.FALLING_EDGE,5)

    #nRFSN.updateStatus()
    nRFSN.clearInt(nRFSN.TX_DS)

# nRFSN.BufOut[0] = random.randint(0, 255)
# nRFSN.BufOut[1] = 0x28
# nRFSN.BufOut[2] = 0xFE
# nRFSN.BufOut[3] = 0xD7
# nRFSN.transmit(4)
#  
# if gpio.wait_for_edge(4,pigpio.FALLING_EDGE,5):
#     nRFSN.updateStatus()
#     nRFSN.clearInt(nRFSN.TX_DS)

nRFSN.updateStatus()
print nRFSN.Status

spi.close()
log.log('nRFSN destroyed')
log.close()
print "Exiting"

exit()
#!/usr/bin/python2.7

import pigpio
import spidev
import SensorNetLog
import nRFSNlib

print "Starting"

spi = spidev.SpiDev()
spi.open(0,0)
spi.max_speed_hz=3000000

gpio = pigpio.pi()

log = SensorNetLog.SensorNetLog()
nRFSN = nRFSNlib.nRFSNlib(25, 24, 8, spi, gpio, log)

nRFSN.BufOut[0] = 0x32
nRFSN.BufOut[1] = 0x28
nRFSN.BufOut[2] = 0xFE
nRFSN.BufOut[3] = 0xD7
nRFSN.transmit(4)

nRFSN.getPayloadSize()

nRFSN.getPayload(4,0)

nRFSN.setRXAddr(nRFSN.RX_ADDR_P0,nRFSN.RX_ADDRESS,4)

nRFSN.configReg('w',0x03,nRFSN.CONFIG_CURR)

nRFSN.close()
log.close()

print "Exiting"

exit()
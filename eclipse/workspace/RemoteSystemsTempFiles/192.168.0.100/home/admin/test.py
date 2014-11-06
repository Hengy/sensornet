#!/usr/bin/python2.7

import SensorNetLog
import nRFSNlib

def main():
    print "Starting"
    
    nRFSN = nRFSNlib.nRFSNlib(25, 24, 8)
    log = SensorNetLog.SensorNetLog()
    
    for i in range(0,5):
        nRFSN.sendByte(0x5a)
    
    log.logSPIBytes('test:',[0x5A,0x5A,0x5A,0x5A,0x5A,0x5A])
    
    nRFSN.close()
    
    print "Exiting"
    
main()
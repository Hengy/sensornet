#!/usr/bin/python2.7

import SensorNetLog
import nRFSNlib

def main():
    print "Starting"
    
    log = SensorNetLog.SensorNetLog()
    nRFSN = nRFSNlib.nRFSNlib(25, 24, 8, log)
    
    #nRFSN.sendByte(0x5a)
    
    nRFSN.sendBytes([0x33,0x28,0xFE])
    
    nRFSN.close()
    
    print "Exiting"
    
main()
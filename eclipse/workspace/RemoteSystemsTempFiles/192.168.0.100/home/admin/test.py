#!/usr/bin/python2.7

import nrf24lib

def main():
    print "Starting"
    
    nrf = nrf24lib.nrf24lib()
    
    for i in range(0,5):
        nrf.sendByte()
    
    nrf.close()
    
    print "Exiting"
    
main()
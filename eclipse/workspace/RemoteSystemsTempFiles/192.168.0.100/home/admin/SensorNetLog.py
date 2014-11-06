#!/usr/bin/python2.7

import datetime
import time

class SensorNetLog:
    
    def __init__(self):
        self.logfile = open('log.txt', 'a')
        self.logfile.write('\nSenorNet testing started:')
        self.logfile.write(datetime.datetime.fromtimestamp(time.time()).strftime('<%Y-%m-%d %H:%M:%S>'))
        self.logfile.write('\n')
        
    def toHex(self, num):
        str = '%02x' % num
        return '0x' + str.upper()
    
    def log(self, text):
        timestamp = datetime.datetime.fromtimestamp(time.time()).strftime('<%Y-%m-%d %H:%M:%S>')
        self.logfile.write('Log: ')
        self.logfile.write(timestamp)
        self.logfile.write(' ')
        self.logfile.write(text)
        self.logfile.write('\n')
        
    def logWarn(self, text):
        timestamp = datetime.datetime.fromtimestamp(time.time()).strftime('<%Y-%m-%d %H:%M:%S>')
        self.logfile.write('Warning: ')
        self.logfile.write(timestamp)
        self.logfile.write(' ')
        self.logfile.write(text)
        self.logfile.write('\n')
        
    def logError(self, text):
        timestamp = datetime.datetime.fromtimestamp(time.time()).strftime('<%Y-%m-%d %H:%M:%S>')
        self.logfile.write('Error: ')
        self.logfile.write(timestamp)
        self.logfile.write(' ')
        self.logfile.write(text)
        self.logfile.write('\n')
        
    def logSPIByte(self, text, byte):
        timestamp = datetime.datetime.fromtimestamp(time.time()).strftime('<%Y-%m-%d %H:%M:%S>')
        line = ['SPI: ',timestamp,' ',text]
        self.logfile.write('SPI: ')
        self.logfile.write(text)
        self.logfile.write(' [')
        self.logfile.write(self.toHex(byte))
        self.logfile.write(']\n')
        
    def logSPIBytes(self, text, *bytes):
        timestamp = datetime.datetime.fromtimestamp(time.time()).strftime('<%Y-%m-%d %H:%M:%S>')
        line = ['SPI: ',timestamp,' ',text]
        self.logfile.write('SPI: ')
        self.logfile.write(text)
        self.logfile.write(' [')
        for i in range(len(str(bytes[0]))/4):
            self.logfile.write(self.toHex(bytes[0][i]))
            if i < (len(str(bytes[0]))/4)-1:
                self.logfile.write(',')
        self.logfile.write(']\n')
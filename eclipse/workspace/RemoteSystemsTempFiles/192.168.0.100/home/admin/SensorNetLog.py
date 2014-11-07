#!/usr/bin/python2.7

import datetime
import time

class SensorNetLog:
    
    def __init__(self):
        self.logfile = open('log.txt', 'a')
        self.logfile.write(datetime.datetime.fromtimestamp(time.time()).strftime('[%Y-%m-%d %H:%M:%S]'))
        self.logfile.write(' ')
        self.logfile.write('SenorNet testing started:')
        self.logfile.write('\n')
        return
        
    def toHex(self, num):
        str = '%02x' % num
        return '0x' + str.upper()
    
    def log(self, text):
        timestamp = datetime.datetime.fromtimestamp(time.time()).strftime('[%Y-%m-%d %H:%M:%S]')
        self.logfile.write(timestamp)
        self.logfile.write(' ')
        self.logfile.write(text)
        self.logfile.write('\n')
        return
        
    def logWarn(self, text):
        timestamp = datetime.datetime.fromtimestamp(time.time()).strftime('[%Y-%m-%d %H:%M:%S]')
        self.logfile.write(timestamp)
        self.logfile.write(' Warning: ')
        self.logfile.write(text)
        self.logfile.write('\n')
        return
        
    def logError(self, text):
        timestamp = datetime.datetime.fromtimestamp(time.time()).strftime('[%Y-%m-%d %H:%M:%S]')
        self.logfile.write(timestamp)
        self.logfile.write(' Error: ')
        self.logfile.write(text)
        self.logfile.write('\n')
        return
        
    def logSPIByte(self, command, byte):
        timestamp = datetime.datetime.fromtimestamp(time.time()).strftime('[%Y-%m-%d %H:%M:%S]')
        self.logfile.write(timestamp)
        self.logfile.write(' SPI transfer: command: [')
        self.logfile.write(self.toHex(command))
        self.logfile.write('] data: [')
        self.logfile.write(self.toHex(byte))
        self.logfile.write(']\n')
        return
        
    def logSPIBytes(self, command, *bytes):
        timestamp = datetime.datetime.fromtimestamp(time.time()).strftime('[%Y-%m-%d %H:%M:%S]')
        self.logfile.write(timestamp)
        self.logfile.write(' SPI transfer: command: [')
        self.logfile.write(self.toHex(command))
        self.logfile.write('] data: [')
        for i in range(len(bytes[0])):
            self.logfile.write(self.toHex(bytes[0][i]))
            if i < len(bytes[0])-1:
                self.logfile.write(',')
        self.logfile.write(']\n')
        return
    
    def close(self):
        self.logfile.close()
        return
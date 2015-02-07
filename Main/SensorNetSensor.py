#--------------------------------------------------------------------------------------
# Author:        Matthew Hengeveld
# Date:          Feb 7, 2015
#--------------------------------------------------------------------------------------
# File:          SesnorNetSesnor.py
# Class:         SensorNetSensor
# Description:   Sesnor class. A sensor is either a sensor or a control.
#--------------------------------------------------------------------------------------

class SensorNetSensor:
    
    command = 0x10                      # command bytes start at 0x10
    
    dataLen = 1                         # length of data in bytes; default of 1 byte
    
    updateInterval = 30                 # default update interval of 30 minutes (twice an hour)
    
    timestamp = 0                       # default timestamp is 0 - no value yet
    
    value = None                        # no default value
    
    warning = [None,None]               # no default warning values
    
    critical = [None,None]              # no default critical values
    
    def __init__(self, command, dataLen, updateInterval, warning, critical):
        self.command = command
        self.dataLen = dataLen
        self.updateInterval = updateInterval
        self.warning = warning
        self.critical = critical
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
    
    value = None                        # no default value
    
    type = 0                            # default type of sensor (0) sensor (1) control
    
    status = 0                          # current status of sensor (0) OK (1) warning (2) critical = (1 if value > warning[1] OR value < warning[0]) OR (2 if value > critical[1] OR value < critical[0])
    
    warning = [None,None]               # no default warning values
    
    critical = [None,None]              # no default critical values
    
    archiveData = None                  # list containing previous values and their timestamps
    
    def __init__(self):
        pass
    
    def printSensor(self):
        print "------------------------------------------------------"
        print "Sensor command:\t\t\t" + hex(self.command).upper()
        print "Sensor data length:\t\t" + str(self.dataLen)
        print "Sensor value:\t\t\t" + str(self.value)
        print "Sensor type:\t\t\t" + str(self.type)
        print "Sensor status:\t\t\t" + str(self.status)
        print "Sensor warning bounds:\t\t[" + str(self.warning[0]) + "," + str(self.warning[1]) + "]"
        print "Sensor critical bounds:\t\t[" + str(self.critical[0]) + "," + str(self.critical[1]) + "]"
        
        return
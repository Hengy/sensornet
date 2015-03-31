#--------------------------------------------------------------------------------------
# Author:        Matthew Hengeveld
# Date:          Feb 7, 2015
#--------------------------------------------------------------------------------------
# File:          SesnorNetModule.py
# Class:         SensorNetModule
# Description:   Module class. Modules are virtual - they are part of a physical Node.
#                Modules are a group of one or more sensors and/of controls that are
#                associated with each other. Modules have a name, description, type,
#                icon, number of sensors and/or controls, and fav status. Modules are 
#                identified (uniquely from other modules) using a combination of their
#                nodes address and the first command byte.
#--------------------------------------------------------------------------------------

class SensorNetModule:
    
    moduleID = 0xE7E7E7E700             # default module ID - combo of Node ID and first command byte
    
    updateInterval = None               # update interval of module - minutes to next value value
    
    updatetime = 0                      # time of next update = (timestamp) + (updateInterval)
    
    timestamp = 0                       # timestamp of last value update
    
    status = 0;                         # status of module = (max status value of sensors[])
    
    sensors = []                        # list of sensors in module
    
    def __init__(self):
        pass
    
    def printModule(self):
        print "------------------------------------------------------"
        print "Module ID:\t\t\t" + hex(self.moduleID).upper()
        print "Module update interval:\t\t\t" + str(self.updateInterval)
        print "Module update time:\t\t\t" + str(self.updatetime)
        print "Module timestamp:\t\t\t" + str(self.timestamp)
        print "Module status:\t\t\t" + str(self.status)
        
        return
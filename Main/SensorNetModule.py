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
from msilib.schema import Icon

class SesnorNetModule:
    
    moduleID = 0xE7E7E7E700             # default module ID - combo of Node ID and first command byte
    
    fav = 0                             # show on home page; (0) no (1) yes
    
    name = "Default"                    # name of module
    
    description = "None."               # description of module and associated sensors and controls
    
    type = 0                            # (0) None (1) sensor (2) control (3) both
    
    icon = "default.png"                # default icon for a module
    
    sensorNum = 0                         # number of sensors in module
    
    controlNum = 0                        # number of controls in module
    
    sensors = []                        # list of sensors in module
    
    def __init__(self):
        pass
    
    def printModule(self):
        print "Module ID: " + str(moduleID)
        print "Fav module? " + str(fav)
        print "Module name: " + name
        print "Module description: " + description
        print "Module type: " + str(type)
        print "Module icon file: " + icon
        print "Module sensors: " + str(sensorNum)
        print "Module controls: " + str(controlNum)
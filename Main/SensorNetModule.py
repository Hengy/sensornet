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

class SesnorNetModule:
    
    def __init__(self):
        pass
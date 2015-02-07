#--------------------------------------------------------------------------------------
# Author:        Matthew Hengeveld
# Date:          Feb 7, 2015
#--------------------------------------------------------------------------------------
# File:          SesnorNetNode.py
# Class:         SensorNetNode
# Description:   Node class. A node is a physical entity, comprised of a microcontroller,
#                and any combination of sensors and/or controls (up to a max of 80).
#                Each node has a location, status, auxillary data, and up to 80 modules.
#                Nodes are identified (uniquely from other nodes) using a 4 byte address.
#                This is the same address used by the nRF24L01+.
#--------------------------------------------------------------------------------------

class SensorNetNode:
    
    ID = 0xE7E7E7E7;              # default node address
    
    status = 0;                   # (0) good (1) warning (2) critical (3) no comm. (>4) custom
    
    location = "None";            # no default location
    
    modules = []
    
    def __init__(self, ID, location):
        self.ID = ID
        self.location = location
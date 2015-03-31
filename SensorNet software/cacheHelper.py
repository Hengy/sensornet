#--------------------------------------------------------------------------------------
# Author:        Matthew Hengeveld
# Date:          Feb 8, 2015
#--------------------------------------------------------------------------------------
# File:          Dataache.py
# Class:         Dataache
# Description:   Implements management and search functions for cached data
#--------------------------------------------------------------------------------------

import py_nRFSN as nRFSN

import struct

class cacheHelper:
    
    cache = None            # cached data
    
    def __init__(self, data):
        self.cache = data
    
    #----------------------------------------------------------------------------------
    # module list class
    #----------------------------------------------------------------------------------
    class module:
        
        moduleID = None
        
        status = 0
        
        def __init__(self, moduleID, status):
            self.moduleID = moduleID
            self.status = status
            
        def printModule(self):
            print "----------------------------------"
            print "ID: " + str(self.moduleID)
            print "Status: " + str(self.status)
    
    #----------------------------------------------------------------------------------
    # get list of modules - names, locations, and icons
    #----------------------------------------------------------------------------------
    def getModuleList(self):
        
        list = []
        
        # get list of all nodes
        for node in self.cache:
            
            # get modules in node
            for module in node.modules:
                
                m = self.module(module.moduleID, node.status)
                
                list.append(m)  # put in list of modules
                
        return list
    
    #----------------------------------------------------------------------------------
    # get information on moduleID
    #----------------------------------------------------------------------------------
    def getModuleInfo(self, moduleID):
        
        moduleData = {
                    'nodeID': None,
                    'moduleID': None,
                    'nodeStatus': None,
                    'updateInterval': None,
                    'timestamp': None,
                    'moduleStatus': None,
                    'commands': [],
                    'dataLens': [],
                    'values': [],
                    'types': [],
                    'status': [],
                    'archiveData': [],
                    'criticalBounds': [],
                    'warningBounds': []
                    }
        
        # get list of all nodes
        for node in self.cache:
            
            # get modules in node
            for module in node.modules:
                
                if (module.moduleID == moduleID):
                    
                    moduleData['nodeID'] = node.ID
                    moduleData['moduleID'] = module.moduleID
                    moduleData['nodeStatus'] = node.status
                    moduleData['updateInterval'] = 10
                    moduleData['timestamp'] = 0
                    moduleData['moduleStatus'] = module.status
                    
                    for sensor in module.sensors:
                        moduleData['commands'].append(sensor.command)
                        moduleData['dataLens'].append(sensor.dataLen)
                        moduleData['values'].append(sensor.value)
                        moduleData['types'].append(sensor.type)
                        moduleData['status'].append(sensor.status)
                        moduleData['archiveData'].append(sensor.archiveData)
                        moduleData['criticalBounds'].append(sensor.warning)
                        moduleData['warningBounds'].append(sensor.critical)
                        
                         # get updated
                        addr = struct.pack(">L", moduleData['nodeID']) #unpack into bytes
                        dataRecvd = []
                        
                        if (sensor.type != 1):
                            for i in xrange(len(moduleData['commands'])):
                                addrList = [ord(addr[0]), ord(addr[1]), ord(addr[2]), ord(addr[3])]
                                dataRecvd.append( nRFSN.py_getSensorVals( addrList, moduleData['commands'][i], moduleData['dataLens'][i] )[0] )
                                moduleData['values'] = dataRecvd
                    
                    break
                

        return moduleData
    
    #----------------------------------------------------------------------------------
    # update values for controls
    #----------------------------------------------------------------------------------
    def updateControlFast(self, data):
         
        # get list of all nodes
        for node in self.cache:
            
            # get modules in node
            for module in node.modules:
                 
                if (module.moduleID == int(data[1])):   # if module ID match is found
                    
                    addr = struct.pack(">L", node.ID) #unpack into bytes
                    addrList = [ord(addr[0]), ord(addr[1]), ord(addr[2]), ord(addr[3])]
                    nRFSN.py_setSensorValsFast( addrList, data[2][0], data[3], 3 )

                    break
        return 1
    
    #----------------------------------------------------------------------------------
    # update values for controls
    #----------------------------------------------------------------------------------
    def updateControl(self, data):
         
        # get list of all nodes
        for node in self.cache:
            
            # get modules in node
            for module in node.modules:
                 
                if (module.moduleID == int(data[1])):   # if module ID match is found
                    
                    addr = struct.pack(">L", node.ID) #unpack into bytes
                    addrList = [ord(addr[0]), ord(addr[1]), ord(addr[2]), ord(addr[3])]
                    nRFSN.py_setSensorValsFast( addrList, data[2][0], data[3], 1 )

                    break
        return 1
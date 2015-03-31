#--------------------------------------------------------------------------------------
# Author:        Matthew Hengeveld
# Date:          Feb 7, 2015
#--------------------------------------------------------------------------------------
# File:          NodeFactory.py
# Class:         NodeFactory
# Description:   NodeFactory class. Creates Node, Module and Sesnor objects from database.
#--------------------------------------------------------------------------------------

import sys

import sqlite3 as sqlite

import DBTools

import SensorNetNode
import SensorNetModule
import SensorNetSensor

class NodeFactory:
    
    nodeCache = []          # list of nodes
    
    dbcur = None
    
    def __init__(self, dbcur):
        self.dbcur = dbcur
    
    
    def getNodes(self):
        
        statement = ('SELECT Node.ID, Node.ModuleID, NodeDetails.Status\n'
        'FROM Node\n'
        'INNER JOIN NodeDetails\n'
        'ON Node.ID=NodeDetails.ID\n'
        'GROUP BY Node.ID;')
        
        data = DBTools.trySelectQuery(self.dbcur, statement, "getNodes()")
        
        return data
    
    
    def getModules(self, ID):
        
        statement = ('SELECT Node.ModuleID, ModuleDetails.UpdateInterval, ModuleDetails.Timestamp, ModuleDetails.Status\n'
        'FROM Node\n'
        'INNER JOIN ModuleDetails\n'
        'ON Node.ModuleID=ModuleDetails.ModuleID\n'
        'WHERE Node.ID=\'{0}\'\n'
        'GROUP BY Node.ModuleID;'.format(str(ID)) )
            
        data = DBTools.trySelectQuery(self.dbcur, str(statement), "getModules()")
        
        return data
    
    
    def getSensors(self, moduleID):
        
        statement = ('SELECT Node.Command, PrimaryData.Value, PrimaryData.DataLen, PrimaryData.Type, PrimaryData.Status, Bounds.WarningMax, Bounds.WarningMin, Bounds.CriticalMax, Bounds.CriticalMin, ArchiveData.Data\n'
        'FROM Node\n'
        'INNER JOIN PrimaryData\n'
        'ON Node.ID=PrimaryData.ID AND Node.Command=PrimaryData.Command\n'
        'INNER JOIN Bounds\n'
        'ON Node.Command=Bounds.Command AND Node.ID=Bounds.ID\n'
        'INNER JOIN ArchiveData\n'
        'ON Node.ID=ArchiveData.ID AND Node.Command=ArchiveData.Command\n'
        'WHERE Node.ModuleID=\'{0}\';'.format(str(moduleID)) )
            
        data = DBTools.trySelectQuery(self.dbcur, str(statement), "getSensors()")
        
        return data
    
        
    def Process(self):
        
        nodes = self.getNodes() # get list of unique nodes (by ID)
        
        for nodei in xrange(len(nodes)):      # for each node
            
            newNode = SensorNetNode.SensorNetNode()           # create new node object            
            newNode.ID = nodes[nodei][0]        # assign ID            
            newNode.status = nodes[nodei][2]    # get status
            
            modules = self.getModules(newNode.ID)    # get list of modules associated with this node
            
            modulesList = []
            for modulei in xrange(len(modules)):     # for each module in the node
            
                newModule = SensorNetModule.SensorNetModule()       # create new module object
                newModule.moduleID = modules[modulei][0]            # assign module ID
                newModule.updateInterval = modules[modulei][1]      # get update interval
                newModule.timestamp = modules[modulei][2]           # get timestamp of previous value
                newModule.status = modules[modulei][3]              # get status
                
                sensors = self.getSensors(newModule.moduleID)    # get list of sensors (by node ID and Command)
                
                sensorsList = []
                for sensori in xrange(len(sensors)): # for each sensor in the module
                    
                    newSensor = SensorNetSensor.SensorNetSensor()               # create new sensor object
                    
                    newSensor.command = sensors[sensori][0]         # assign command byte
                    newSensor.value = sensors[sensori][1]           # assign value
                    newSensor.dataLen = sensors[sensori][2]         # assign data length
                    newSensor.type = sensors[sensori][3]            # assign sensor type
                    newSensor.status = sensors[sensori][4]          # get status of sensor
                    newSensor.warning = [sensors[sensori][5],sensors[sensori][6]]   # get warning value
                    newSensor.critical = [sensors[sensori][7],sensors[sensori][8]]  # get critical values
                    #newSensor.archiveData = sensors[sensori][9]     # get archive data
                    
                    sensorsList.append(newSensor)
                    #newSensor.printSensor()
        
                newModule.sensors = sensorsList # store list of sensors in its module
                
                modulesList.append(newModule)
                #newModule.printModule()
            
            newNode.modules = modulesList   # store list of modules in its node
        
            self.nodeCache.append(newNode)   # store node
            #newNode.printNode()
            
        return self.nodeCache
    
    
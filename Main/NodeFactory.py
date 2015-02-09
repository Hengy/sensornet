#--------------------------------------------------------------------------------------
# Author:        Matthew Hengeveld
# Date:          Feb 7, 2015
#--------------------------------------------------------------------------------------
# File:          NodeFactory.py
# Class:         NodeFactory
# Description:   NodeFactory class. Creates Node, Module and Sesnor objects from database.
#--------------------------------------------------------------------------------------

class NodeFactory:
    
    nodeCache = []          # list of nodes
    
    dbcur = None            # database cursor
    
    def __init__(self, dbcur):
        self.dbcur = dbcur
        
        
    def Process(self):
        
        nodes = getnodes() # get list of unique nodes (by ID)
        
        for nodei in xrange(nodes):      # for each node
            
            newNode = SensorNetNode()           # create new node object            
            newNode.ID = nodes[nodei][0]        # assign ID            
            newNode.location = node[nodei][1]   # assign location
            
            modules = getModules(newNode.ID)    # get list of modules associated with this node
            
            modulesList = []
            for modulei in xrange(modules):     # for each module in the node
            
                newModule = SensorNetModule()               # create new module object
                newModule.moduleID = modules[modulei][1]    # assign module ID
                newModule.name = modules[modulei][2]        # assign name
                newModule.description = modules[modulei][3] # assign description
                newModule.type = modules[modulei][4]        # assign type
                newModule.icon = modules[modulei][5]        # assign icon file name
                newModule.fav = modules[modulei][6]         # assign fav boolean
                
                sensors = getSensors(newModule.moduleID)    # get list of sensors (by node ID and Command)
                
                sensorsList = []
                for sensori in xrange(sensors): # for each sensor in the module
                    
                    newSensor = SensorNetSensor()               # create new sensor object
                    
                    newSensor.command = sensors[sensori][0]         # assign command byte
                    newSensor.value = sensors[sensori][1]           # assign value
                    newSensor.dataLen = sensors[sensori][2]         # assign data length
                    newSensor.updateInterval = sensors[sensori][3]  # assign update interval
                    newSensor.timestamp = sensors[sensori][4]       # assign timestamp
                    newSensor.type = sensors[sensori][5]            # assign sensor type
                    newSensor.warning = [sensors[sensori][6],sensors[sensori][7]]   # get warning value
                    newSensor.critical = [sensors[sensori][8],sensors[sensori][9]]  # get critical values
        
                newModule.sensors = sensorsList # store list of sensors in its module
            
            newNode.modules = modulesList   # store list of modules in its node
        
            nodeCache.append(newNode)   # store node
            
        return nodeCache
    
    def getNodes(self):
        try:
            dbcur.execute(  '''SELECT Node.ID, Node.ModuleID, NodeDetails.Location
                            FROM Node
                            INNER JOIN NodeDetails
                            ON Node.ID=NodeDetails.ID
                            GROUP BY Node.ID;
                            ''')
            
            data = dbcur.fetchall()
                
        except sqlite.Error, e:
            
            print "getNodes() Error %s: " % e.args[0]
            sys.exit(1)
        
        return data
    
    
    def getModules(self, ID):
        try:
            dbcur.execute(  '''SELECT Node.ID, Node.ModuleID, ModuleDetails.Name, ModuleDetails.Description, ModuleDetails.Type, ModuleDetails.Icon, Fav.Fav
                            FROM Node
                            WHERE Node.ID=''' + str(ID) + '''
                            INNER JOIN ModuleDetails
                            ON Node.ModuleID=ModuleDetails.ModuleID
                            INNER JOIN Fav
                            ON Node.Command=Fav.Command AND Node.ID=Fav.ID;
                            ''')
            
            data = dbcur.fetchall()
                
        except sqlite.Error, e:
            
            print "getModules() Error %s: " % e.args[0]
            sys.exit(1)
        
        return data[0]
    
    def getSensors(self, moduleID):
        try:
            dbcur.execute(  '''SELECT Node.Command, PrimaryData.Value, PrimaryData.DataLen, PrimaryData.UpdateInterval, PrimaryData.Timestamp, PrimaryData.Type, Bounds.WarningMax, Bounds.WarningMin, Bounds.CriticalMax, Bounds.CriticalMin, ArchiveData.Data
                            FROM Node
                            WHERE Node.ModuleID=''' + str(moduleID) + '''
                            INNER JOIN PrimaryData
                            ON Node.ID=PrimaryData.ID AND Node.Command=PrimaryData.Command
                            INNER JOIN Bounds
                            ON Node.Command=Bounds.Command AND Node.ID=Bounds.ID
                            INNER JOIN ArchiveData
                            ON Node.ID=ArchiveData.ID AND Node.Command=ArchiveData.Command;
                            ''')
            
            data = dbcur.fetchall()
                
        except sqlite.Error, e:
            
            print "getSensors() Error %s: " % e.args[0]
            sys.exit(1)
        
        return data
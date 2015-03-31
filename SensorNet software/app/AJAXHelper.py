import cPickle as pickle

import sqlite3 as sqlite

import zmq

#----------------------------------------------------------------------------------
# Get data for index page
#----------------------------------------------------------------------------------
def getIndexModuleList(dbcur):
    
    #----------------------------------------------------------------------------------
    # Get module list
    #----------------------------------------------------------------------------------
    statement = ('SELECT Node.ID, Node.ModuleID, NodeDetails.Location, ModuleDetails.Name, ModuleDetails.Icon \n'
        'FROM Node\n'
        'INNER JOIN NodeDetails\n'
        'ON Node.ID=NodeDetails.ID\n'
        'INNER JOIN ModuleDetails\n'
        'ON Node.ModuleID=ModuleDetails.ModuleID\n'
        'GROUP BY Node.moduleID;')
    
    try:
        dbcur.execute(statement)
        
        moduleListData = dbcur.fetchall()
            
    except sqlite.Error, e:
        
        print "Module List Error %s: " % e.args[0]
        sys.exit(1)
    
    return moduleListData


#----------------------------------------------------------------------------------
# Get data for index page
#----------------------------------------------------------------------------------
def getIndexCardList(dbcur):
    
    #----------------------------------------------------------------------------------
    # get card list
    #----------------------------------------------------------------------------------
                #        0                1                    2                3                    4                    5                        6                    7                    8                    9                10                    11            12
    statement = ('SELECT Node.ID, PrimaryData.Command, Node.ModuleID, NodeDetails.Location, NodeDetails.Status, ModuleDetails.ModuleFile, ModuleDetails.Name, ModuleDetails.Icon, ModuleDetails.Status, PrimaryData.DataLen, PrimaryData.Value, PrimaryData.Status, Fav.Fav \n'
        'FROM Node\n'
        'INNER JOIN NodeDetails\n'
        'ON Node.ID=NodeDetails.ID\n'
        'INNER JOIN ModuleDetails\n'
        'ON Node.ModuleID=ModuleDetails.ModuleID\n'
        'INNER JOIN PrimaryData\n'
        'ON Node.ID=PrimaryData.ID AND Node.Command=PrimaryData.Command\n'
        'INNER JOIN Fav\n'
        'ON Node.ModuleID=Fav.ModuleID\n'
        'WHERE Fav.Fav=1 OR NodeDetails.Status>0 OR ModuleDetails.Status>0 OR PrimaryData.Status>0\n'
        'GROUP BY Node.ModuleID;')
    
    try:
        dbcur.execute(statement)
        
        cardListData = dbcur.fetchall()
            
    except sqlite.Error, e:
        
        print "Card List Error %s: " % e.args[0]
        sys.exit(1)
        
    return cardListData


#----------------------------------------------------------------------------------
# Get data for index page
#----------------------------------------------------------------------------------
def getIndexStatsList(dbcur):
        
    #----------------------------------------------------------------------------------
    # get stats
    #----------------------------------------------------------------------------------
    statement = ('SELECT * FROM Stats;')
    
    try:
        dbcur.execute(statement)
        
        statsData = dbcur.fetchall()
            
    except sqlite.Error, e:
        
        print " Error %s: " % e.args[0]
        sys.exit(1)
        
    return statsData
    
#----------------------------------------------------------------------------------
# Get module
#----------------------------------------------------------------------------------
def loadID(dbcur, socket, moduleID):
    
    querySuccess = True
    
    moduleData = {
                  'querySuccess': True,
                  'moduleID': None,
                  'ID': None,
                  'nodeStatus': None,
                  'name': None,
                  'description': None,
                  'moduleType': None,
                  'icon': None,
                  'updateInterval': None,
                  'timestamp': None,
                  'moduleStatus': None,
                  'moduleFile': None,
                  'commands': [],
                  'dataLens': [],
                  'values': [],
                  'types': [],
                  'status': [],
                  'archiveData': [],
                  'criticalBounds': [],
                  'warningBounds': []
                  }
    
    # get variables that aren't cached
    statement = ('SELECT Node.ID, Node.ModuleID, ModuleDetails.Name, ModuleDetails.Description, ModuleDetails.Type, ModuleDetails.Icon, ModuleDetails.ModuleFile \n'
        'FROM Node\n'
        'INNER JOIN ModuleDetails\n'
        'ON Node.ModuleID=ModuleDetails.ModuleID\n'
        'WHERE Node.ModuleID={0};'.format(str(moduleID)))
    
    try:
        dbcur.execute(statement)
        
        moduleData_db = dbcur.fetchall()
        
        if len(moduleData_db) > 0:
        
            moduleData['moduleID'] = moduleData_db[0][1]
            moduleData['ID'] = moduleData_db[0][0]
            moduleData['name'] = moduleData_db[0][2]
            moduleData['description'] = moduleData_db[0][3]
            moduleData['moduleType'] = moduleData_db[0][4]
            moduleData['icon'] = moduleData_db[0][5]
            moduleData['moduleFile'] = moduleData_db[0][6]
            
        else:
            
            querySuccess = False
            
    except sqlite.Error, e:
        
        print "Module Error %s: " % e.args[0]
        sys.exit(1)
    
    if (querySuccess):
    
        SensorNet_request = ['get', moduleID]
    
        socket.send(pickle.dumps(SensorNet_request, pickle.HIGHEST_PROTOCOL))
        
        # create and register sockets for polling
        SesnorNetPoller = zmq.Poller()
        SesnorNetPoller.register(socket, zmq.POLLIN)   # Poll for messages from nRFSN
        
        timeout = 40        
        while timeout > 0:
        
            socks = dict(SesnorNetPoller.poll()) # poll all sockets
            
            # If reply from nRFSN process
            if socket in socks and socks[socket] == zmq.POLLIN:
        
                depickledData = pickle.loads(socket.recv())
        
                if depickledData['moduleID'] == moduleID:
                    moduleData['ID'] = depickledData['nodeID']
                    moduleData['nodeStatus'] = depickledData['nodeStatus']
                    moduleData['updateInterval'] = depickledData['updateInterval']
                    moduleData['timestamp'] = depickledData['timestamp']
                    moduleData['moduleStatus'] = depickledData['moduleStatus']
                    moduleData['commands'] = depickledData['commands']
                    moduleData['dataLens'] = depickledData['dataLens']
                    moduleData['values'] = depickledData['values']
                    moduleData['types'] = depickledData['types']
                    moduleData['status'] = depickledData['status']
                    moduleData['archiveData'] = depickledData['archiveData']
                    moduleData['criticalBounds'] = depickledData['criticalBounds']
                    moduleData['warningBounds'] = depickledData['warningBounds']
                else:
                    querySuccess = False
                
                break
            
            --timeout
        
        if timeout < 1:
            querySuccess = False
            
    moduleData['querySuccess'] = querySuccess
    
    return moduleData

#----------------------------------------------------------------------------------
#  Update control - update vallues in module
#----------------------------------------------------------------------------------
def updateControlFast(socket, data):
    
    querySuccess = True
     
    #query SensorNet process; return success
    SensorNet_request = ['setFast', data['moduleID'], data['commands'], data['values']]
     
    socket.send(pickle.dumps(SensorNet_request, pickle.HIGHEST_PROTOCOL))
     
    # create and register sockets for polling
    SesnorNetPoller = zmq.Poller()
    SesnorNetPoller.register(socket, zmq.POLLIN)   # Poll for messages from nRFSN
     
    timeout = 40        
    while timeout > 0:
     
        socks = dict(SesnorNetPoller.poll()) # poll all sockets
         
        # If reply from nRFSN process
        if socket in socks and socks[socket] == zmq.POLLIN:
             
            depickledData = pickle.loads(socket.recv())
             
            #check for errors
            querySuccess = int(depickledData)
             
            break
         
    if timeout < 1:
        querySuccess = False   
    
    return querySuccess


#----------------------------------------------------------------------------------
#  Update control - update vallues in module
#----------------------------------------------------------------------------------
def updateControl(socket, data):
    
    querySuccess = True
     
    #query SensorNet process; return success
    SensorNet_request = ['set', data['moduleID'], data['commands'], data['values']]
     
    socket.send(pickle.dumps(SensorNet_request, pickle.HIGHEST_PROTOCOL))
     
    # create and register sockets for polling
    SesnorNetPoller = zmq.Poller()
    SesnorNetPoller.register(socket, zmq.POLLIN)   # Poll for messages from nRFSN
     
    timeout = 40        
    while timeout > 0:
     
        socks = dict(SesnorNetPoller.poll()) # poll all sockets
         
        # If reply from nRFSN process
        if socket in socks and socks[socket] == zmq.POLLIN:
             
            depickledData = pickle.loads(socket.recv())
             
            #check for errors
            querySuccess = int(depickledData)
             
            break
         
    if timeout < 1:
        querySuccess = False   
    
    return querySuccess
    
    
#----------------------------------------------------------------------------------
# Get module
#----------------------------------------------------------------------------------
def updateModule(dbcur, socket, moduleID):
    
    querySuccess = True
    
    return querySuccess
    
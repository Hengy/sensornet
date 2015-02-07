#--------------------------------------------------------------------------------------
# Author:        Matthew Hengeveld
# Date:          Feb 7, 2015
#--------------------------------------------------------------------------------------
# File:          NodeFactory.py
# Class:         NodeFactory
# Description:   NodeFactory class. Creates Node, Module and Sesnor objects from database.
#--------------------------------------------------------------------------------------

class NodeFactory:
    
    nodes = []              # list of nodes
    
    dbcur = None            # database cursor
    
    def __init__(self, dbcur):
        self.dbcur = dbcur
        
        nodeData = getNode()
        
        for row in xrange(nodeData):
            
            location = getLocation(nodeData[row][0])
            
            newNode = SensorNetNode(nodeData[row][0], location)
            
            nodes.append(newNode)
        
    
    def getNode(self):
        try:
            dbcur.execute("SELECT * FROM Node")
            
            data = dbcur.fetchall()
                
        except sqlite.Error, e:
            
            print "getNode() Error %s: " % e.args[0]
            sys.exit(1)
        
        return data
    
    
    def getLocation(self, ID):
        try:
            dbcur.execute("SELECT Location FROM NodeDetails WHERE ID=" + str(ID))
            
            data = dbcur.fetchall()
                
        except sqlite.Error, e:
            
            print "getLocation() Error %s: " % e.args[0]
            sys.exit(1)
        
        return data[0]
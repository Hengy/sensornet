#--------------------------------------------------------------------------------------
# Author:        Matthew Hengeveld
# Date:          Mar 4, 2015
#--------------------------------------------------------------------------------------
# File:          nRFSNRequester.py
# Description:   Handles requests to and replies from nRFSN process
#--------------------------------------------------------------------------------------
import cPickle as pickle

def ping(socket):
    
    request("ping", None, socket)


def requestModule(moduleID, socket):
    
    request("get", [moduleID, command], socket)
    

def controlModule(moduleID, commands, values, socket):
    
    request("set", [moduleID, commands, values], socket)
    
def controlModuleFast(moduleID, commands, values, socket):
    
    request("setFast", [moduleID, commands, values], socket)


def request(type, data, socket):

    if data is not None:
        pickledData = pickle.dumps(data.insert(0, type), pickle.HIGHEST_PROTOCOL)
    else:
        pickledData = pickle.dumps([type], pickle.HIGHEST_PROTOCOL)
        
    socket.send(pickledData)


def replyFrom(socket):
    
    data = pickle.loads(socket.recv())
    
    return data

def kill(socket):
    
    request("die", None, socket)
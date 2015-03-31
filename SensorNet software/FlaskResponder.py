#--------------------------------------------------------------------------------------
# Author:        Matthew Hengeveld
# Date:          Mar 4, 2015
#--------------------------------------------------------------------------------------
# File:          FlaskResponder.py
# Description:   Handles requests from and replies to Flask app
#--------------------------------------------------------------------------------------
import cPickle as pickle

def requestFrom(socket, cache):
    
    msg = pickle.loads(socket.recv())
    
    print "FlaskResponder: {0}".format(msg)
    
    if (msg[0] == 'get'):
        pickleData = pickle.dumps(cache.getModuleInfo(msg[1]), pickle.HIGHEST_PROTOCOL)
        socket.send(pickleData)
    elif (msg[0] == 'set'):
        success = cache.updateControl(msg)
        pickleData = pickle.dumps(1, pickle.HIGHEST_PROTOCOL)
        socket.send(pickleData)
    elif (msg[0] == 'setFast'):
        success = cache.updateControlFast(msg)
        pickleData = pickle.dumps(1, pickle.HIGHEST_PROTOCOL)
        socket.send(pickleData)
    else:
        pickleData = pickle.dumps("NULL", pickle.HIGHEST_PROTOCOL)
        socket.send(pickleData) 
    
    return


def reply(data, socket):
    
    if (type is not None):
        pickledData = pickle.dumps(data.insert(0, type), pickle.HIGHEST_PROTOCOL)
    else:
        pickledData = pickle.dumps(data, pickle.HIGHEST_PROTOCOL)


def replyFrom(socket):
    
    data = pickle.loads(socket.recv())
    
    return data
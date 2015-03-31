#!/usr/bin/python
#--------------------------------------------------------------------------------------
# Author:        Matthew Hengeveld
# Date:          Feb 7, 2015
#--------------------------------------------------------------------------------------
# File:          SensorNet.py
# Description:   Main SensorNet program. Manages local copies of module data. Handles
#                schedules for updates. Schedules updates of database. Manages
#                nRF24L01+ transceiver. Manages other SensorNet processes.
#--------------------------------------------------------------------------------------


#--------------------------------------------------------------------------------------
# Imports
#--------------------------------------------------------------------------------------
import sqlite3 as sqlite        # import database library

import sys                      # import system library
import signal                   # SIG handler

import time                     # import time and date libraries
import datetime

import cPickle as pickle        # import data serialization library pickle. Use C version

import NodeFactory              # import node factory for copying database

import cacheHelper              # import dataCache for searching functions

import zmq                      # import ZeroMQ messaging (MOM)

import FlaskResponder           # handle requests from and replies to Flask app

import py_nRFSN as nRFSN

from app import app             # Flask app


#--------------------------------------------------------------------------------------
# Global variables
#--------------------------------------------------------------------------------------
dbconn = None                   # database connection
dbcur = None                    # database cursor

cache = None                    # cache of node data

exitFlag = 0                    # flag for signals

#--------------------------------------------------------------------------------------
# Main
#--------------------------------------------------------------------------------------
def main(args):
    
    port = 5001     # default port number
    
    # check if other port number if specified
    if len(args) > 0:
        for i in xrange(len(args)):
            if args[i] == "-p":
                if str(args[i+1]).isdigit():
                    port = args[i+1]
                break

    print "Starting SensorNet"
    
    #----------------------------------------------------------------------------------
    # Database setup
    #     Get database connection and cursor
    #----------------------------------------------------------------------------------
    try:
        dbconn = sqlite.connect('database/sensornet.s3db')
        
        dbcur = dbconn.cursor()
        
    except sqlite.Error, e:
        
        print "Database connection Error %s: " % e.args[0]
            
        sys.exit(1)
    
    
    #----------------------------------------------------------------------------------
    # Local copy of database
    #     Local copies are made to reduce RPi SD card write and wear
    #----------------------------------------------------------------------------------
    nf = NodeFactory.NodeFactory(dbcur)         # create node factory
     
    cache = cacheHelper.cacheHelper(nf.Process())   # create cache using node factory data
    
    #----------------------------------------------------------------------------------
    # Handler for SIGNALs
    #----------------------------------------------------------------------------------
    signal.signal(signal.SIGINT, sigHandler)
    
    #----------------------------------------------------------------------------------
    # ZeroMQ for flask app and nRFSN interface process
    #----------------------------------------------------------------------------------

    # Create ZMQ socket and connect to Flask app
    port = 5001
    Sensornet_Flask_socket = ZMQSocket(port, zmq.REP) # socket from Flask app   

    #----------------------------------------------------------------------------------
    # nRF24L01+ interface
    #----------------------------------------------------------------------------------
    nRFSN.py_nRFSN_init(64,25,4,17)	# 64 SPI divider; pins 25, 4, 17 for nRF24L01+
    
    #----------------------------------------------------------------------------------
    # Main loop
    #    Poll sockets
    #        Get values for web interface
    #    Get scheduled updates
    #----------------------------------------------------------------------------------
    
    # create and register sockets for polling
    mainPoller = zmq.Poller()
    mainPoller.register(Sensornet_Flask_socket, zmq.POLLIN)   # Poll for messages from flask
    
    while not exitFlag:
        
        socks = dict(mainPoller.poll()) # poll all sockets
                
        # If request from Flask app
        if Sensornet_Flask_socket in socks and socks[Sensornet_Flask_socket] == zmq.POLLIN:
            
            FlaskResponder.requestFrom(Sensornet_Flask_socket, cache)    # handle request from Flask app
            
        time.sleep(0.025)
        
    exitGracefully(Sensornet_Flask_socket)

    sys.exit(0)
    
    #----------------------------------------------------------------------------------
    # Exit procedures
    #----------------------------------------------------------------------------------
    
def exitGracefully(socketflask):
# def exitGracefully(socketnRFSN, socketflask, processnRFSN):
    
    # Close database connection
    dbconn.close()
    
    # stop
    nRFSN.py_stop()
    
    # Close socket to Flask app
    socketflask.close()
	
    # Wait for nRFSNInterface process to exit
    processnRFSN.join()
        
    print "Exiting SensorNet."
    
    sys.exit(0)

#--------------------------------------------------------------------------------------
# Open ZeroMQ socket
#--------------------------------------------------------------------------------------
def ZMQSocket(port, type):
    
    context = zmq.Context()
    
    socket = context.socket(type)
    
    if type == zmq.REQ:
        socket.bind('tcp://127.0.0.1:' + str(port))
    else:
        socket.connect('tcp://127.0.0.1:' + str(port))
    
    return socket

#--------------------------------------------------------------------------------------
# nRFSN process
#--------------------------------------------------------------------------------------  
def sigHandler(signal, frame):
    
    global exitFlag
    
    print "Exiting gracefully..."
    
    exitFlag = 1
    
    sys.exit(0)

#--------------------------------------------------------------------------------------
# init function
#--------------------------------------------------------------------------------------  
if __name__=='__main__':
    main(sys.argv)  # call main

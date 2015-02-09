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

import array                    # import array

import time                     # import time and date libraries
import datetime

import cPickle as pickle        # import data serialization library pickle. Use C version

import SensorNetNode            # import node and module libraries
import SensorNetModule

import NodeFactory              # import node factory for copying database

import DataCache                # import dataCache for searching functions


#--------------------------------------------------------------------------------------
# Global variables
#--------------------------------------------------------------------------------------
VERBOSE_MODE = False        # verbose mode

dbconn = None               # database connection
dbcur = None                # database cursor

cache = None            # cache of node data


#--------------------------------------------------------------------------------------
# Main
#--------------------------------------------------------------------------------------
def main(argv):
    
    print "Starting SensorNet."
    
    #----------------------------------------------------------------------------------
    # Command line arguments
    #----------------------------------------------------------------------------------
    if (len(argv) > 1):
        
        # If there are command line arguments
        if "-v" in argv:
            VERBOSE_MODE = True # turn on verbose mode
            print "Verbose mode is on." 
    
    
    #----------------------------------------------------------------------------------
    # Database setup
    #     Get database connection and cursor
    #----------------------------------------------------------------------------------
    try:
        dbconn = sql.connect('database/sensornet.s3db')
        
        dbcur = dbconn.cursor()
        dbcur.execute('SELECT SQLITE_VERSION()')
        
        data = dbcur.fetchone()
        
        if (VERBOSE_MODE):
            print "SQLite version: %s" % data
        
    except sqlite.Error, e:
        
        if (VERBOSE_MODE):
            print "Database connection Error %s: " % e.args[0]
            
        sys.exit(1)
    
    
    #----------------------------------------------------------------------------------
    # Local copy of database
    #     Local copies are made to reduce RPi SD card write and wear
    #----------------------------------------------------------------------------------
    nf = NodeFactory(dbcur)         # create node factory
    
    cache = DataCache(nf.Process()) # create cache using node factory data
    
    
    
    print "Exiting SensorNet."

if __name__ == '__main__':
    main(sys.argv)
    
    
    
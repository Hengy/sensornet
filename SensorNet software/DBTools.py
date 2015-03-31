#--------------------------------------------------------------------------------------
# Author:        Matthew Hengeveld
# Date:          Feb 9, 2015
#--------------------------------------------------------------------------------------
# File:          DBTools.py
# Class:         DBTools
# Description:   Provides common functions for interfacing with database
#--------------------------------------------------------------------------------------

import sys

import sqlite3 as sqlite

def trySelectQuery(dbcur, statement, queryName):
    try:
        dbcur.execute(statement)
        
        data = dbcur.fetchall()
            
    except sqlite.Error, e:
        
        print queryName + " Error %s: " % e.args[0]
        sys.exit(1)
    
    return data
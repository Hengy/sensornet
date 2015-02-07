#--------------------------------------------------------------------------------------
# Author:        Matthew Hengeveld
# Date:          Feb 7, 2015
#--------------------------------------------------------------------------------------
# File:          sensornet.py
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



if __name__ == '__main__':
    main()
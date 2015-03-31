#--------------------------------------------------------------------------------------
# Author:        Matthew Hengeveld
# Date:          Feb 7, 2015
#--------------------------------------------------------------------------------------
# File:          views.py
# Description:   Views for Sensornet
#--------------------------------------------------------------------------------------

#--------------------------------------------------------------------------------------
# Global imports
#--------------------------------------------------------------------------------------
from flask import render_template, g, request
from app import app

import sqlite3 as sqlite

import json

import AJAXHelper

import zmq                      # import ZeroMQ messaging (MOM)

import cPickle as pickle        # import data serialization library pickle. Use C version

#--------------------------------------------------------------------------------------
# Connect to database
#--------------------------------------------------------------------------------------
def connectDB():
    db = getattr(g, '_database', None)
    if db is None:
        try:
            db = sqlite.connect('database/sensornet.s3db')  
            g._database = db
        except sqlite.Error, e:
            print "Fatal: Database connection Error %s: " % e.args[0]
            sys.exit(1)
    return db

#--------------------------------------------------------------------------------------
# Connect to database
#--------------------------------------------------------------------------------------
def connectZMQ():
    
    context = zmq.Context()    
    socket = context.socket(zmq.REQ)    
    socket.bind('tcp://127.0.0.1:5001')   
    g._socket = socket
    
    return socket

#--------------------------------------------------------------------------------------
# App teardown
#--------------------------------------------------------------------------------------
@app.teardown_appcontext
def teardown(Exception):
    
    #close database
    db = getattr(g, '_database', None)
    if db is not None:
        db.close()
        
    #close socket
    socket = getattr(g, '_socket', None)
    if socket is not None:
        socket.close()

#--------------------------------------------------------------------------------------
# Ajax - get module list
#--------------------------------------------------------------------------------------
@app.route('/getModuleList', methods=['GET'])
def getModuleList():
    # get DB cursor
    dbcur = connectDB().cursor()
    
    # get requested list name
    data = request.args.get("list")

    # get all data for index page
    list = AJAXHelper.getIndexModuleList(dbcur)
    
    return json.dumps(list)       # return json

#--------------------------------------------------------------------------------------
# Ajax - get card list
#--------------------------------------------------------------------------------------
@app.route('/getCardList', methods=['GET'])
def getCardList():
    # get DB cursor
    dbcur = connectDB().cursor()
    
    # get all data for index page
    list = AJAXHelper.getIndexCardList(dbcur)
    
    return json.dumps(list)       # return json

#--------------------------------------------------------------------------------------
# Ajax - get stats list
#--------------------------------------------------------------------------------------
@app.route('/getStatsList', methods=['GET'])
def getStatsList():
    # get DB cursor
    dbcur = connectDB().cursor()
    
    # get all data for index page
    list = AJAXHelper.getIndexStatsList(dbcur)
    
    return json.dumps(list)       # return json

#--------------------------------------------------------------------------------------
# Ajax - get module info
#--------------------------------------------------------------------------------------
@app.route('/getModuleInfo', methods=['GET'])
def getModuleInfo():
    # get DB cursor
    dbcur = connectDB().cursor()
    
    # get requested list name
    data = request.args.get("moduleID")

    # get all data for index page using sqlite db and ZMQ socket to sensorent process
    moduleInfo = AJAXHelper.loadID(dbcur, connectZMQ(), long(data))
    
    return json.dumps(moduleInfo)       # return json


#--------------------------------------------------------------------------------------
# Ajax - update automation control(s); update values only 
# - no settings (only updates cached value in SensorNet process)
#--------------------------------------------------------------------------------------
@app.route('/updateControl', methods=['POST'])
def updateControl():
    success = False
    
    # get node ID, moduleID, and value
    #data = request.form.get('ID')
    data = request.get_json()

    # get all data for index page using sqlite db and ZMQ socket to sensorent process
    success = AJAXHelper.updateControl(connectZMQ(), data)
    
    return json.dumps(success)       # return success of update in json


#--------------------------------------------------------------------------------------
# Ajax - update automation control(s); update values only 
# - no settings (only updates cached value in SensorNet process)
#--------------------------------------------------------------------------------------
@app.route('/updateControlFast', methods=['POST'])
def updateControlFast():
    success = False
    
    # get node ID, moduleID, and value
    #data = request.form.get('ID')
    data = request.get_json()

    # get all data for index page using sqlite db and ZMQ socket to sensorent process
    success = AJAXHelper.updateControlFast(connectZMQ(), data)
    
    return json.dumps(success)       # return success of update in json

#--------------------------------------------------------------------------------------
# Ajax - update automation control(s); update values only 
# - no settings (only updates cached value in SensorNet process)
#--------------------------------------------------------------------------------------
@app.route('/updateModule', methods=['POST'])
def updateModule():
    success = False
    
    # get DB cursor
    dbcur = connectDB().cursor()
    
    # get requested list name
    data = request.get_json()

    # get all data for index page using sqlite db and ZMQ socket to sensorent process
    #success = AJAXHelper.updateModule(dbcur, connectZMQ(), data)
    
    return json.dumps(success)       # return success of update in json

#--------------------------------------------------------------------------------------
# Index route
#--------------------------------------------------------------------------------------
@app.route('/')
@app.route('/index')
def index():
    
    return render_template('index.html')

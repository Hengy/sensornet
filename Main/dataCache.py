#--------------------------------------------------------------------------------------
# Author:        Matthew Hengeveld
# Date:          Feb 9, 2015
#--------------------------------------------------------------------------------------
# File:          Dataache.py
# Class:         Dataache
# Description:   Implements management and search functions for cached data
#--------------------------------------------------------------------------------------

class DataCache:
    
    cache = None            # cached data
    
    def __init__(self, data):
        self.cache = data
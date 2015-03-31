import py_nRFSN as p
import time

p.py_nRFSN_init(64,25,4,17)
 
count = 0
k = 20
while(k >= 0):
    
    count += 1
    if ((count % 10) == 0):
        print count
        
    p.py_setSensorValsFast([0xCA,0xCA,0xCA,0xCA], 0x10, [255,0,0], 3)
    time.sleep(0.1)
    p.py_setSensorValsFast([0xCA,0xCA,0xCA,0xCA], 0x10, [0,255,0], 3)
    time.sleep(0.1)
    p.py_setSensorValsFast([0xCA,0xCA,0xCA,0xCA], 0x10, [0,0,255], 3)
    time.sleep(0.1)
    
# p.py_setSensorValsFast([0xCA,0xCA,0xCA,0xCA], 0x10, [255,0,255], 3)

p.py_stop()
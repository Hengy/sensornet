cimport defns

cimport cython

from libc.stdlib cimport malloc, free

#----------------------------------------------------------------------------------
# C nRFSN Wrapper code
#----------------------------------------------------------------------------------
def py_nRFSN_init(char SPIDiv, char CE, char IRQ, char LED):
    defns.nRFSN_init(SPIDiv, CE, IRQ, LED)
    
def py_setTXMode():
    defns.setTXMode()
    
def py_setRXMode():
    defns.setTXMode()
    
def py_getMode():
    return defns.getMode()

def py_setPower(char pwr):
    defns.setPower(pwr)
    
def py_getPower():
    return defns.getPower()
    
def py_setChannel(char ch):
    defns.setChannel(ch)
    
def py_getChannel():
    return defns.getChannel()
    
def py_setMaxRT(char max):
    defns.setMaxRT(max)
    
def py_getMaxRT():
    return defns.getMaxRT()
    
def py_setMaxRTdelay(char delay):
    defns.setMaxRTdelay(delay)
    
def py_getMaxRTdelay():
    return defns.getMaxRTdelay()
    
def py_setTXAddr(addr, char len):
    cdef char *addrptr
    addrptr = <char *>malloc(len)
    for i in xrange(len):
        addrptr[i] = addr[i]
    defns.setTXAddr(addrptr, len)
    free(addrptr) # free memory after function is complete
    
def py_setRXAddr(char pipe, addr, char len):
    cdef char *addrptr
    addrptr = <char *>malloc(len)
    for i in xrange(len):
        addrptr[i] = addr[i]
    defns.setRXAddr(pipe, addrptr, len)
    free(addrptr) # free memory after function is complete
    
def py_getTXAddr():
    cdef char* addr = defns.getTXAddr()
    return addr
    
def py_getRXAddr(char pipe):
    cdef char* addr = defns.getRXAddr(pipe)
    return addr

def py_clearInt(char intr):
    defns.clearInt(intr)
    
def py_updateStatus():
    return defns.updateStatus()

def py_transmit(char len):
    defns.transmit(len)
    
def py_getPayloadSize():
    return defns.getPayloadSize()
    
def py_getPayload(char len):
    defns.getPayload(len)
    
def py_putBufOut(buf, char len):
    cdef char *bufptr
    bufptr = <char *>malloc(len)
    for i in xrange(len):
        bufptr[i] = buf[i]
    defns.putBufOut(bufptr, len)
    free(bufptr) # free memory after function is complete
    
def py_putSingleBufOut(char data):
    defns.putSingleBufOut(data)
    
def py_getBufIn(char len):
    cdef char* data = defns.getBufIn(len)
    return data
    
def py_flush(int buf):  # (0) RX (1) TX
    defns.flush(buf)
    
def py_bcmDelay(int ms):
    defns.bcmDelay(ms)
    
def py_bcmDelayM(int us):
    defns.bcmDelay(us)
    
def py_stop():
    defns.stop()
    
def py_getSensorVals(addr, command, len):
    # addr list to array
    cdef char *addrptr
    addrptr = <char *>malloc(4)
    for i in xrange(4):
        addrptr[i] = addr[i]
    
    # execute function
    cdef char *returnedData = defns.getSensorVals(addrptr, command, len)
    
    py_data = []
    for i in xrange(len):
        py_data.append(returnedData[i])
    
    return py_data

def py_setSensorVals(addr, command, data, len):
    # addr list to array
    cdef char *addrptr
    addrptr = <char *>malloc(4)
    for i in xrange(4):
        addrptr[i] = addr[i]
    
    # command and data list to array
    cdef char *dataptr
    dataptr = <char *>malloc(len+1)
    dataptr[0] = command
    for i in xrange(len):
        dataptr[i+1] = data[i]
    
    # execute function
    defns.setSensorVals(addrptr, dataptr, len)
    
def py_setSensorValsFast(addr, command, data, len):
    # addr list to array
    cdef char *addrptr
    addrptr = <char *>malloc(4)
    for i in xrange(4):
        addrptr[i] = addr[i]
    
    # command and data list to array
    cdef char *dataptr
    dataptr = <char *>malloc(len+1)
    dataptr[0] = command
    for i in xrange(len):
        dataptr[i+1] = data[i]
    
    # execute function
    defns.setSensorValsFast(addrptr, dataptr, len)
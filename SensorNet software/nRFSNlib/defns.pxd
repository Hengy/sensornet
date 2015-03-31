cdef extern from "nRFSN.h":
    void nRFSN_init(char, char, char, char)
    void stop()
    
    void setTXMode()
    void setRXMode()
    char getMode()
     
    void setPower(char)
    char getPower()
    void setChannel(char)
    char getChannel()
    void setMaxRT(char)
    char getMaxRT()
    void setMaxRTdelay(char)
    char getMaxRTdelay()
     
    void setTXAddr(char*, char)
    void setRXAddr(char, char*, char)
    char *getTXAddr()
    char *getRXAddr(char pipe)
     
    void clearInt(char)
    char updateStatus()
     
    void transmit(char len)
    char getPayloadSize()
    void getPayload(char)
     
    void putBufOut(char*, char)
    void putSingleBufOut(char)
    char *getBufIn(char)
    
    void flush(int)
    
    void bcmDelay(int)
    void bcmDelayM(int)
    
    char *getSensorVals(char*, char, char)
    void setSensorVals(char*, char*, char)
    void setSensorValsFast(char*, char*, char)
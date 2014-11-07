#!/usr/bin/python2.7

#-------------------------------------------------
# nRF24L01+ test program for raspberry pi (rev 2, Debian)
# Uses spidev and pigpio libraries for spi and gpio, resp.
#-------------------------------------------------

import pigpio
import time
import atexit

class nRFSNlib:
    
    # nRF24L01+ commands
    R_REGISTER = 0x00       # Read; Bits <5:0> = register map address (LSB first)
    W_REGISTER = 0x20       # Write; Bits <5:0> = register map address (LSB first)
    R_RX_PAYLOAD = 0x61     # Read RX payload 1-32 bytes (LSB first)
    W_TX_PAYLOAD = 0xA0     # Write TX payload 1-32 bytes (LSB first)
    FLUSH_TX = 0xE1         # Flush TX FIFO
    FLUSH_RX = 0xE2         # Flush RX FIFO
    REUSE_TX_PL = 0xE3      # TX; Reuse last transmitted payload; active until FLUSH_TX or W_TX_PAYLOAD
    R_RX_PL_WID = 0x60      # Read RX payload width for top R_RX_PAYLOAD in RX FIFO
    W_ACK_PAYLOAD = 0xA8    # RX; Write payload + ACK packet; <2:0> = write payload (LSB first)
    W_TX_PAYLOAD_NO = 0xB0  # TX; Disable AUTOACK on this specific packet
    NRF_NOP = 0xFF          # No operation; used as dummy data
    
    # nRF24L01+ registers
    CONFIG = 0x00           # Configuration register
    EN_AA = 0x01            # Enable AUTOACK function
    EN_RXADDR = 0x02        # Enable RX addresses
    SETUP_AW = 0x03         # Setup address widths
    SETUP_RETR = 0x04       # Setup auto retransmission
    RF_CH = 0x05            # RF channel
    RF_SETUP = 0x06         # RF setup register
    STATUS = 0x07           # Status register
    OBSERVE_TX = 0x08       # Transmit observe register
    RPD = 0x09              # RPD (Carrier Detect)
    RX_ADDR_P0 = 0x0A       # Receive address data for pipes 0-5
    RX_ADDR_P1 = 0x0B
    RX_ADDR_P2 = 0x0C
    RX_ADDR_P3 = 0x0D
    RX_ADDR_P4 = 0x0E
    RX_ADDR_P5 = 0x0F
    TX_ADDR = 0x10          # Transmit address
    RX_PW_P0 = 0x11         # Receive data width for pipes 0-5
    RX_PW_P1 = 0x12
    RX_PW_P2 = 0x13
    RX_PW_P3 = 0x14
    RX_PW_P4 = 0x15
    RX_PW_P5 = 0x16
    FIFO_STATUS = 0x17      # FIFO status register
    DYNPD = 0x1C            # Enable dynamic payload length
    FEATURE = 0x1D          # Feature register
    
    # nRF24L01+ interrupts
    RX_DR = 0x40            # Data received interrupt
    TX_DS = 0x20            # Data sent interrupt
    MAX_RT = 0x10           # Max retransmit interrupt
    
    #------------------------------------------------
    # nRFSN command definitions
    #------------------------------------------------
    # Sensor commands ("Request sensor value")
    SENV_0 = 0x10
    SENV_1 = 0x11
    SENV_2 = 0x12
    SENV_3 = 0x13
    SENV_4 = 0x14
    SENV_5 = 0x15
    SENV_6 = 0x16
    SENV_7 = 0x17
    SENV_8 = 0x18
    SENV_9 = 0x19
    SENV_10 = 0x1A
    SENV_11 = 0x1B
    SENV_12 = 0x1C
    SENV_13 = 0x1D
    SENV_14 = 0x1E
    SENV_15 = 0x1F
    
    # Automation commands ("Update automation value")
    AUTOV_0 = 0x20
    AUTOV_1 = 0x21
    AUTOV_2 = 0x22
    AUTOV_3 = 0x23
    AUTOV_4 = 0x24
    AUTOV_5 = 0x25
    AUTOV_6 = 0x26
    AUTOV_7 = 0x27
    AUTOV_8 = 0x28
    AUTOV_9 = 0x29
    AUTOV_10 = 0x2A
    AUTOV_11 = 0x2B
    AUTOV_12 = 0x2C
    AUTOV_13 = 0x2D
    AUTOV_14 = 0x2E
    AUTOV_15 = 0x2F
    
    # System commands
    BATT_LVL = 0x30    # Request VCC voltage level
    CURR_LVL = 0x31    # Request current consumption
    
    # nRF defaults
    CONFIG_CURR     = 0b00101011   #Show RX_DR and MAX_RT interrupts; Enable CRC - 1 uint8_t; Power up; RX
    EN_AA_CURR      = 0b00000011   #Enable Auto Ack for pipe 0,1
    EN_RXADDR_CURR  = 0b00000011   #Enable data pipe 0,1
    SETUP_AW_CURR   = 0b00000010   #Set up for 4 address
    SETUP_RETR_CURR = 0b00110000   #1000us retransmit delay; 10 retransmits
    RF_CH_CURR      = 0b01101001   #Channel 105 (2.400GHz + 0.105GHz = 2.505GHz)
    RF_SETUP_CURR   = 0b00000110   #RF data rate to 1Mbps; 0dBm output power (highest)
    RX_PW_P0_CURR   = 0b00000001   #1 payload
    DYNPD_CURR      = 0b00000011   #Set dynamic payload for pipe 0
    FEATURE_CURR    = 0b00000100   #Enable dynamic payload
    RX_ADDRESS      = [0xE7,0xE7,0xE7,0xE7]
    TX_ADDRESS      = [0xE7,0xE7,0xE7,0xE7]
    
    Status = 0     # nRF24L01+ STATUS register 
    
    Busy = False   # nRF busy flag; set when transmitting

    RXInt = False  # nRF received packet flag set by ISR
    TXInt = False  # nRF packet sent flag set by ISR
    MAXInt = False # nRF max retransmit flag set by ISR
    
    manualCSN = False # specifies if CSN pin is triggered by spidev or by manual pin manipulation
    
    # Buffers
    BufIn = [0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
             0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
             0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
             0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff]
    BufOut = [0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
              0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
              0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
              0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff]
    
    def __init__(self, CEpin, IRQpin, CSNpin, spi, gpio, log):
        self.log = log
        self.log.log('nRFSN created')
        
        #self.gpio = pigpio.pi()
        self.gpio = gpio
        
        self.CEpin = CEpin
        self.IRQpin = IRQpin
        self.gpio.set_mode(CEpin, pigpio.OUTPUT)
        self.gpio.set_mode(IRQpin, pigpio.INPUT)
        self.gpio.write(CEpin, 0)
        
        if CSNpin == 8:
            self.CSNpin = 8
        elif CSNpin == 7:
            self.CSNpin = 8
            self.log.logWarn('SPIDEV(0,0) must use pin 8 for CSN. Pin automatically changed.')

        self.spi = spi
        #self.spi = spidev.SpiDev()
        #self.spi.open(0,0)
        #self.spi.max_speed_hz=3000000
        return
    
    def setPower(self, pwrLvl):
        self.RF_SETUP_CURR = pwrLvl << 1;
        self.configReg('w',self.RF_SETUP,self.RF_SETUP_CURR)
        return
    
    def setTXMode(self):
        self.CONFIG_CURR = 0b01001010
        self.configReg('w',self.CONFIG,self.CONFIG_CURR)
        return
    
    def setRXMode(self):
        self.CONFIG_CURR = 0b00101011
        self.configReg('w',self.CONFIG,self.CONFIG_CURR)
        return
        
    def setMAX_RT(self, numRT):
        self.SETUP_RETR_CURR = (self.SETUP_RETR_CURR & 0b11110000) | (numRT & 0b00001111)
        self.configReg('w',self.SETUP_RETR,self.SETUP_RETR_CURR)
        return
        
    def setChannel(self, ch):
        self.RF_CH_CURR = ch;
        self.configReg('w',self.RF_CH,self.RF_CH_CURR)
        return
        
    def transfer(self, wrn, command, dataLen, offset):
        total = dataLen + offset
        if total > 28:
            total = 28

        if wrn == 'w':
            cmdByte = self.W_REGISTER|command
        elif wrn == 'r':
            cmdByte = self.R_REGISTER|command
        else:
            cmdByte = command
            
        data = self.spi.xfer2([cmdByte] + self.BufOut[offset:total])
        self.log.logSPIBytes(cmdByte,self.BufOut[offset:total])

        if dataLen:
            for i in range(total):
                self.BufIn[i] = data[i]
        return
    
    def transmit(self, dataLen):
        if dataLen:
            self.spi.xfer2([0xA0] + self.BufOut[0:dataLen])

            self.gpio.gpio_trigger(self.CEpin, 11, 1)
            
            self.Busy = True;
        return
    
    def getPayloadSize(self):
        data = self.spi.xfer2([self.R_RX_PL_WID, self.NRF_NOP])
        return data[1]
    
    def getPayload(self, payloadSize, offset):
        self.transfer('r',self.R_RX_PAYLOAD,payloadSize,offset)
        return
    
    def configReg(self, wr, command, data):
        if wr == 'w':
            cmdByte = self.W_REGISTER|command
        elif wr == 'r':
            cmdByte = self.R_REGISTER|command
            data = self.NRF_NOP
            
        data = self.spi.xfer2([cmdByte, data])
        return data[1]

    def setTXAddr(self, addr, addrLen):
        if addrLen:
            data = self.spi.xfer2([self.W_REGISTER|self.TX_ADDR] + addr[0:addrLen])
        return
    
    def setRXAddr(self, pipe, addr, addrLen):
        if addrLen:
            data = self.spi.xfer2([self.W_REGISTER|pipe] + addr[0:addrLen])
        return
    
    def updateStatus(self):
        self.Status = self.spi.xfer([self.NRF_NOP])
        return

    def close(self):
        self.spi.close()
        self.log.log('nRFSN destroyed')
        return
        
        
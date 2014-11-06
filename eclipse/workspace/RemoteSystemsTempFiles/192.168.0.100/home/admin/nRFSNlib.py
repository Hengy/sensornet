#!/usr/bin/python2.7

#-------------------------------------------------
# nRF24L01+ test program for raspberry pi (rev 2, Debian)
# Uses spidev and pigpio libraries for spi and gpio, resp.
#-------------------------------------------------

import pigpio
import spidev
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
    
    def __init__(self, CEpin, IRQpin, CSNpin):
        print "Welcome to the nRF24L01+ Python library on SensorNet."
        
        self.CEpin = CEpin
        self.IRQpin = IRQpin
        if CSNpin == 8:
            self.CSNpin = 8
        elif CSNpin == 7:
            self.CSNpin = 8
        else:
            self.CSNpin = CSNpin
        
        self.spi = spidev.SpiDev()
        self.spi.open(0,0)

    def sendByte(self, byte):
        print "Executing SPI send byte"
        self.spi.xfer2([byte])

    def close(self):
        print "Closing..."
        self.spi.close()
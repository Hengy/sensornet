
/*------------------------------------------------
 * File:        nRF_RX
 * Version:     0.1.0
 * --------------------------------------------
 * Author:      Matthew Hengeveld
 * Date:        September 9, 2014
 * --------------------------------------------
 * uProc:       Arduino (ATmega328P)
 *   Fosc:        16MHz
 * Compiler:    Arduino IDE 1.0.5r2
 * --------------------------------------------
 * Description:
 *      Tests nRF24L01+ transmitting using Arduino Uno
------------------------------------------------*/

/*------------------------------------------------
 * Naming convention
 *  function:   funcName()
 *  variables:  varName
 *  constants:  CONST_NAME
------------------------------------------------*/

/*------------------------------------------------
 * SPI pins
 *   MOSI:  11
 *   MISO:  12
 *   SCK:   13
------------------------------------------------*/

// SPI library
#include <SPI.h>

/*------------------------------------------------
 * Pin Definitions
------------------------------------------------*/
// LEDs
const int actLED     = 6;    // Activity LED

// nRF24L01+ pins
const int nRF_CE     = 5;    // nRF24L01+ chip enable
const int nRF_CSN    = 4;    // nRF24L01+ chip select not
const int nRF_IRQ    = 3;    // nRF24L01+ interrupt

/*------------------------------------------------
 * nRF24L01+ command and register definitions
------------------------------------------------*/
// nRF24L01+ commands
const byte R_REGISTER      = 0x00;        // Read; Bits <5:0> = register map address (LSB first)
const byte W_REGISTER      = 0x20;        // Write; Bits <5:0> = register map address (LSB first)
const byte R_RX_PAYLOAD    = 0x61;        // Read RX payload 1-32 bytes (LSB first)
const byte W_TX_PAYLOAD    = 0xA0;        // Write TX payload 1-32 bytes (LSB first)
const byte FLUSH_TX        = 0xE1;        // Flush TX FIFO
const byte FLUSH_RX        = 0xE2;        // Flush RX FIFO
const byte REUSE_TX_PL     = 0xE3;        // TX; Reuse last transmitted payload; active until FLUSH_TX or W_TX_PAYLOAD
const byte R_RX_PL_WID     = 0x60;        // Read RX payload width for top R_RX_PAYLOAD in RX FIFO
const byte W_ACK_PAYLOAD   = 0xA8;        // RX; Write payload + ACK packet; <2:0> = write payload (LSB first)
const byte W_TX_PAYLOAD_NO = 0xB0;        // TX; Disable AUTOACK on this specific packet
const byte NRF_NOP         = 0xFF;        // No operation

// nRF24L01+ registers
const byte CONFIG          = 0x00;        // Configuration register
const byte EN_AA           = 0x01;        // Enable AUTOACK function
const byte EN_RXADDR       = 0x02;        // Enable RX addresses
const byte SETUP_AW        = 0x03;        // Setup address widths
const byte SETUP_RETR      = 0x04;        // Setup auto retransmission
const byte RF_CH           = 0x05;        // RF channel
const byte RF_SETUP        = 0x06;        // RF setup register
const byte STATUS          = 0x07;        // Status register
const byte OBSERVE_TX      = 0x08;        // Transmit observe register
const byte RPD             = 0x09;        // RPD (Carrier Detect)
const byte RX_ADDR_P0      = 0x0A;        // Receive address data for pipes 0-5
const byte RX_ADDR_P1      = 0x0B;
const byte RX_ADDR_P2      = 0x0C;
const byte RX_ADDR_P3      = 0x0D;
const byte RX_ADDR_P4      = 0x0E;
const byte RX_ADDR_P5      = 0x0F;
const byte TX_ADDR         = 0x10;        // Transmit address
const byte RX_PW_P0        = 0x11;        // Receive data width for pipes 0-5
const byte RX_PW_P1        = 0x12;
const byte RX_PW_P2        = 0x13;
const byte RX_PW_P3        = 0x14;
const byte RX_PW_P4        = 0x15;
const byte RX_PW_P5        = 0x16;
const byte FIFO_STATUS     = 0x17;        // FIFO status register
const byte DYNPD           = 0x1C;        // Enable dynamic payload length
const byte FEATURE         = 0x1D;        // Feature register


/*------------------------------------------------
 * Misc definitions
------------------------------------------------*/


/*------------------------------------------------
 * nRF24L01+ current config settings
------------------------------------------------*/
byte CONFIG_CURR           = B00101011;   // Show RX_DR and MAX_RT interrupts; Enable CRC - 1 byte; Power up; RX
byte EN_AA_CURR            = B00000001;   // Enable Auto Ack for pipe 0
byte EN_RXADDR_CURR        = B00000011;   // Enable data pipe 0,1
byte SETUP_AW_CURR         = B00000010;   // Set up for 4 byte address
byte SETUP_RETR_CURR       = B00100000;   // 750us retransmit delay; Disable auto retransmit
byte RF_CH_CURR            = B01101001;   // Channel 105 (2.400GHz + 0.105GHz = 2.505GHz)
byte RF_SETUP_CURR         = B00000110;   // RF data rate to 1Mbps; 0dBm output power (highest)
byte RX_PW_P0_CURR         = B00000001;   // 1 byte payload
byte RX_ADDRESS[4]         = {0xC7,0xC7,0xC7,0xC7};
byte TX_ADDRESS[4]         = {0xC7,0xC7,0xC7,0xC7};


/*------------------------------------------------
 * Global variables
------------------------------------------------*/
byte dataBufIn[32];              // 32 byte buffer for all incoming SPI data
byte dataBufOut[32];             // 32 byte buffer for all outgoing SPI data

byte nrfSTATUS;                  // Latest STATUS of nRF

volatile boolean intRXData;      // nRF received packet flag; set by ISR

/*------------------------------------------------
 * Setup
------------------------------------------------*/
void setup() {
  // Pin modes
  pinMode(actLED, OUTPUT);                // Set LED pin to output and initialize to 0
  digitalWrite(actLED, LOW);
  
  pinMode(nRF_IRQ, INPUT);                // Set IRQ pin to input
  pinMode(nRF_CSN, OUTPUT);               // Set CSN pin to output
  pinMode(nRF_CE, OUTPUT);                // Set chip enable pin to output and initialize to 0
  digitalWrite(nRF_CE, HIGH);             // CE must be high when receiving
  digitalWrite(nRF_CSN, HIGH);
  
  // Serial setup - TESTING
  Serial.begin(57600);
  Serial.println("Receiving...");
  
  // SPI setup
  SPI.setBitOrder(MSBFIRST);              // Set most significant bit first
  SPI.setClockDivider(SPI_CLOCK_DIV32);   // Clock to Fosc/16 = 1MHz
  //SPI.setDataMode(SPI_MODE1);             // Clock polarity 0; clock phase 1
  SPI.begin();                            // Start SPI
  
  delay(1);
  
  // nRF24L01+ setup
  // Write to CONFIG register
  nrfConfigReg('w',CONFIG,CONFIG_CURR);
  // Write to EN_RXADDR register  
  nrfConfigReg('w',EN_RXADDR,EN_RXADDR_CURR);
  // Write to EN_AA register
  nrfConfigReg('w',EN_AA,EN_AA_CURR);
  // Write to SETUP_AW register
  nrfConfigReg('w',SETUP_AW,SETUP_AW_CURR);
  // Write to SETUP_RETR register
  nrfConfigReg('w',SETUP_RETR,SETUP_RETR_CURR);
  // Write to RF channel register
  nrfConfigReg('w',RF_CH,RF_CH_CURR);
  // Write to RF setup register  
  nrfConfigReg('w',RF_SETUP,RF_SETUP_CURR);
  // set TX address
  nrfSetTXAddr(TX_ADDRESS,4);
  // set RX address
  nrfSetRXAddr(RX_ADDR_P0,RX_ADDRESS,4);
  // Write pipe 0 payload width
  nrfConfigReg('w',RX_PW_P0,RX_PW_P0_CURR);
  // Flush RX FIFO
  spiTransfer('n',FLUSH_RX,0);
  // Flush TX FIFO
  spiTransfer('n',FLUSH_TX,0);
  
  delay(1);
  
  intRXData = false;
  attachInterrupt(1,ISR_RXData,LOW);
}

/*------------------------------------------------
 * Main
 * Polls nRF24L01+ STATUS register and lights
 * LED when data received
------------------------------------------------*/
void loop() {
  
  digitalWrite(nRF_CE, HIGH);            // Keep CE high when receiving
  
  nrfGetStatus();
  
  if (intRXData == true) {
    
    digitalWrite(nRF_CE, LOW);           // Keep CE high when receiving
    
    spiTransfer('r',R_RX_PAYLOAD,1);          // Read payload command

    Serial.println(dataBufIn[0]);            // Print to serial monitor
    
    dataBufOut[0] = B01110000;
    spiTransfer('w',STATUS,1);
    
    spiTransfer('n',FLUSH_RX,0);
    
    intRXData = false;
  }
  
}

/*------------------------------------------------
 * ISR
------------------------------------------------*/
void ISR_RXData() {
  intRXData = true;
}

/*------------------------------------------------
 * Sets CSN pin HIGH or LOW w/ delay
------------------------------------------------*/
void setCSN(boolean level) {
  if (level) {
    digitalWrite(nRF_CSN, HIGH);  // Set HIGH
  } else {
    digitalWrite(nRF_CSN, LOW);   // Set LOW
  }
}

/*------------------------------------------------
 * Gets STATUS reg from nRF
------------------------------------------------*/
void nrfGetStatus() {
  
  setCSN(LOW);
  
  nrfSTATUS = spiTransferByte(NRF_NOP);
  
  setCSN(HIGH);
}

/*------------------------------------------------
 * Writes or eads nRF reg
------------------------------------------------*/
byte nrfConfigReg(char wr, byte command, byte data) {

    setCSN(LOW);                                // Set CSN low

    if (wr == 'w') {                            // Write
        spiTransferByte(W_REGISTER|command);    // Send command
        delayMicroseconds(3);
        spiTransferByte(data);
    } else if (wr == 'r') {                     // Read
        spiTransferByte(R_REGISTER|command);    // Send command
        delayMicroseconds(3);
        data = spiTransferByte(NRF_NOP);
    }

    setCSN(HIGH);                               // Set CSN high

    return data;
}

/*------------------------------------------------
 * Writes new transmit address to nRF
 * addr[] - array of bytes containing address
 * len - length of address (in bytes)
------------------------------------------------*/
void nrfSetTXAddr(byte addr[], int len) {

    setCSN(LOW);                            // Set CSN low

    spiTransferByte(W_REGISTER|TX_ADDR);    // Send command

    delayMicroseconds(3);

    if (len != 0) {
        // Send address bytes
        for (int i=1;i<=len;i++) {
            spiTransferByte(addr[i-1]);
            delayMicroseconds(3);
        }
    }

    setCSN(HIGH);                           // Set CSN high
}

/*------------------------------------------------
 * Writes new receive address to nRF
 * pipe - which pipe to write address to
 * addr[] - array of bytes containing address
 * len - length of address (in bytes)
------------------------------------------------*/
void nrfSetRXAddr(byte pipe, byte addr[], int len) {

    setCSN(LOW);                        // Set CSN low

    spiTransferByte(W_REGISTER|pipe);   // Send command

    delayMicroseconds(3);

    if (len != 0) {
        // Send address bytes
        for (int i=1;i<=len;i++) {
            spiTransferByte(addr[i-1]);
            delayMicroseconds(3);
        }
    }

    setCSN(HIGH);                       // Set CSN high
}

/*------------------------------------------------
 * SPI transfer function: send and receive multiple bytes w/ command
 * wrn - 'read', 'write' or 'neither'
 * command - unsigned char
 * len - size of data
 * send data - stored in dataBufOut[]
 * receive data - stored in dataBufIn[]
------------------------------------------------*/
void spiTransfer(char wrn, byte command,int len) {
    
    setCSN(LOW);                                // Set CSN low

    if (wrn == 'w') {                           // Write
        spiTransferByte(W_REGISTER|command);    // Send command
    } else if (wrn == 'r') {                    // Read
        spiTransferByte(R_REGISTER|command);    // Send command
    } else if(wrn == 'n') {
        spiTransferByte(command);               // Send command
    }
    //delayMicroseconds(3);

    if (len != 0) {
        for (int i=1;i<=len;i++) {
            dataBufIn[i-1] = spiTransferByte(dataBufOut[i-1]);
            //delayMicroseconds(3);
        }
    }

    setCSN(HIGH);                               // Set CSN high
}

/*------------------------------------------------
 * SPI transfer function: send and receive 1 byte
------------------------------------------------*/
byte spiTransferByte(byte data) {

  SPDR = data;                            // Send data
  while (!(SPSR & (1<<SPIF))) {           // Wait until the end of transmission
  };

  return SPDR;                            // return recieved data
}


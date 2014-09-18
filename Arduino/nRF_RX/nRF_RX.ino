
/*------------------------------------------------
 * File:        nRF_RX
 * Version:     0.0.1
 * --------------------------------------------
 * Author:      Matthew Hengeveld
 * Date:        August 10, 2014
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
const int actLED     = 5;    // Activity LED

// nRF24L01+ pins
const int nRF_CE     = 4;    // nRF24L01+ chip enable
const int nRF_CSN    = 3;    // nRF24L01+ chip select not
const int nRF_IRQ    = 2;    // nRF24L01+ interrupt

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
const byte DUMMY_DATA      = 0xCC;        // Dummy data for SPI (0b11001100)


/*------------------------------------------------
 * nRF24L01+ current config settings
------------------------------------------------*/
byte CONFIG_CURR           = B00001011;   // Show all interrupts; Enable CRC - 1 byte; Power up; RX
byte EN_ARXADDR_CURR       = B00000010;   // Enable data pipe 1
byte SETUP_AW_CURR         = B00000010;   // set up for 4 byte address
byte RF_CH_CURR            = B01101001;   // Channel 105 (2.400GHz + 0.105GHz = 2.505GHz)
byte RF_SETUP_CURR         = B00000110;   // RF data rate to 1Mbps; 0dBm output power (highest)
byte RX_ADDRESS[4]         = {0xE7,0xE7,0xE7,0xE7};
byte TX_ADDRESS[4]         = {0xE7,0xE7,0xE7,0xE7};


/*------------------------------------------------
 * Global variables
------------------------------------------------*/
byte dataBufIn[32];                           // 32 byte buffer for all incoming SPI data
byte dataBufOut[32];                          // 32 byte buffer for all outgoing SPI data


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
  digitalWrite(nRF_CE, LOW);
  digitalWrite(nRF_CSN, HIGH);
  
  // SPI setup
  SPI.begin();                            // Start SPI
  SPI.setBitOrder(MSBFIRST);              // Set most significant bit first
  SPI.setClockDivider(SPI_CLOCK_DIV16);   // Clock to Fosc/16 = 1MHz
  SPI.setDataMode(SPI_MODE1);             // Clock polarity 0; clock phase 1
  
  // nRF24L01+ setup
  spiWrite(W_REGISTER|CONFIG);            // Write to CONFIG register
  spiWrite(CONFIG_CURR);
  spiWrite(W_REGISTER|SETUP_AW);          // Write to SETUP_AW register
  spiWrite(SETUP_AW_CURR);
  
  spiWrite(W_REGISTER|RF_CH);             // Write to RF channel register
  spiWrite(RF_CH_CURR);
  spiWrite(W_REGISTER|RF_SETUP);          // Write to RF setup register
  spiWrite(RF_SETUP_CURR);
  spiWrite(FLUSH_TX);                     // Flush TX FIFO
}

/*------------------------------------------------
 * Main
 * Polls nRF24L01+ STATUS register and lights
 * LED when data received
------------------------------------------------*/
void loop() {

  byte nRFStatus = getSTATUS();
  
  if (bitRead(nRFStatus,7)) {
    digitalWrite(actLED, HIGH);
  }
  
}


/*------------------------------------------------
 * SPI write function; toggles CSN pin for nRF24L01+
------------------------------------------------*/
byte spiTransfer(byte data) {
  
  // toggle CSN pin
  digitalWrite(nRF_CSN, LOW);
  
  SPDR = data;                         // Send NOP command
  while (!(SPSR & (1<<SPIF))) {           // Wait until the end of transmission
  };
  
  digitalWrite(nRF_CSN, HIGH);
  
  return SPDR;
}


/*------------------------------------------------
 * Get STATUS register from nRF24L01+
 * Sends NOP commmand and gets STATUS reg
 * returns char STATUS register
------------------------------------------------*/
byte getSTATUS(void) {
  
  // toggle CSN pin
  digitalWrite(nRF_CSN, LOW);
  
  SPDR = NRF_NOP;                         // Send NOP command
  while (!(SPSR & (1<<SPIF))) {           // Wait until the end of transmission
  };
  
  digitalWrite(nRF_CSN, HIGH);
  
  return SPDR;                            // return the received byte
}


/*------------------------------------------------
 * nRF24L01+ set TX address
 * char addr - 4 byte address
------------------------------------------------*/
void nrfSetTXAddr(byte addr[]) {
    spiWrite(W_REGISTER|TX_ADDR);       // Set new transmit address

    int i;                              // Send all 4 bytes
    for (i=0;i<4;i++) {
        spiWrite(addr[i]);
    }
}

/*------------------------------------------------
 * nRF24L01+ set RX for pipe address
 * char addrXX - which pipe to set address for
 * char addr - 4 byte address
------------------------------------------------*/
void nrfSetRXAddr(byte addrXX, byte addr[]) {
    spiWrite(W_REGISTER|addrXX);        // Set new receive address
    
    int i;                              // Send all 4 bytes
    for (i=0;i<4;i++) {
        spiWrite(addr[i]);
    }
}


/*------------------------------------------------
 * nnRF24L01+ send function
------------------------------------------------*/
void nrfTX(byte data) {
  spiWrite(W_TX_PAYLOAD);                 // Write to TX payload register
  spiWrite(data);                         // Write data
  
  // Toggle CE pin
  digitalWrite(nRF_CE, HIGH);
  delayMicroseconds(10);
  digitalWrite(nRF_CE, LOW);
}

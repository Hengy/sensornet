
/*------------------------------------------------
 * File:        nRF_TX
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
const byte actLed     = 5;    // Activity LED

// nRF24L01+ pins
const byte nRF_CE     = 4;    // nRF24L01+ chip enable
const byte nRF_CSN    = 3;    // nRF24L01+ chip select not
const byte nRF_IRQ    = 2;    // nRF24L01+ interrupt

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
 * nRF24L01+ initial config settings
------------------------------------------------*/
byte CONFIG_INIT           = B00001010;
byte EN_ARXADDR_INIT       = B00000010;
byte SETUP_AW_INIT         = B00000010;
byte RF_CH_INIT            = B01101001;
byte RF_SETUP_INIT         = B00000110;
byte RX_ADDRESS[4]         = {0xE7,0xE7,0xE7,0xE7};
byte TX_ADDRESS[4]         = {0xE7,0xE7,0xE7,0xE7};

/*------------------------------------------------
 * Setup
------------------------------------------------*/
void setup() {
  // Pin modes
  pinMode(actLed, OUTPUT);                // Set LED pin to output and initialize to 0
  digitalWrite(actLed, LOW);
  
  pinMode(nRF_IRQ, INPUT);                // Set IRQ pin to input
  pinMode(nRF_CSN, OUTPUT);               // Set CSN pin to output
  pinMode(nRF_CE, OUTPUT);                // Set chip enable pin to output and initialize to 0
  digitalWrite(nRF_CE, LOW);
  
  // SPI setup
  SPI.begin();                            // Start SPI
  SPI.setBitOrder(MSBFIRST);              // Set most significant bit first
  SPI.setClockDivider(SPI_CLOCK_DIV32);   // Clock to Fosc/32 = 500KHz
  SPI.setDataMode(SPI_MODE1);             // Clock polarity 0; clock phase 1
  
  // nRF24L01+ setup
  SPI.transfer(W_REGISTER|CONFIG);        // Write to CONFIG register
  SPI.transfer(B00001010);                // Show all interrupts; Enable CRC - 1 byte; Power up; TX
  SPI.transfer(W_REGISTER|SETUP_AW);      // Write to SETUP_AW register
  SPI.transfer(B00000001);                // 3 byte address
  SPI.transfer(W_REGISTER|RF_CH);         // Write to RF channel register
  SPI.transfer(B01101001);                // Channel 105 (2.400GHz + 0.105GHz = 2.505GHz)
  SPI.transfer(W_REGISTER|RF_SETUP);      // Write to RF setup register
  SPI.transfer(B00000110);                // RF data rate to 1Mbps; 0dBm output power (highest)
  SPI.transfer(FLUSH_TX);                 // Flush TX FIFO
}

/*------------------------------------------------
 * Main
------------------------------------------------*/
void loop() {
  
}

/*------------------------------------------------
 * nnRF24L01+ send function
------------------------------------------------*/
void nrfTX(int data) {
  SPI.transfer(W_TX_PAYLOAD);             // Write to TX payload register
  SPI.transfer(data);                     // Write data
  
  // Toggle CE pin
  digitalWrite(nRF_CE, HIGH);
  delayMicroseconds(20);
  digitalWrite(nRF_CE, LOW);
}

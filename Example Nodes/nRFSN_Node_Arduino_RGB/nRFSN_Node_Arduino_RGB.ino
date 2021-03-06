//-----------------------------------
// Standard nRFSN include - These MUST NOT CHANGE in order for nRFSN to work!
//-----------------------------------
#include <nRFSN.h>
//-----------------------------------
// Includes
//-----------------------------------
#include <SPI.h>        // SPI library
#include <EEPROM.h>     // EEPROM library
//-----------------------------------

//-----------------------------------
// Definitions
//-----------------------------------
#define nRF_CE   18    // chip enable
#define nRF_CSN  19    // chip select
#define nRF_IRQ  20    // irq

#define Rpin  9
#define Gpin  6
#define Bpin  5

//-----------------------------------

//-----------------------------------
// Global variables
//-----------------------------------
int r,g,b;

int max_rgb_val= 255;

uint8_t nodeAddr[4] = {0xCA,0xCA,0xCA,0xCA};
uint8_t *buf = (uint8_t*)calloc(4, sizeof(uint8_t));
//-----------------------------------

//-----------------------------------
// Arduino setup loop
//-----------------------------------
void setup() {
//  Serial.begin(115200);
//  
//  Serial.println("Start");
  
  nRFSN_setup();
  
  pinMode(Rpin, OUTPUT);
  pinMode(Gpin, OUTPUT);
  pinMode(Bpin, OUTPUT);
  
  analogWrite(Rpin, 10);
  analogWrite(Gpin, 10);
  analogWrite(Bpin, 10);
  
  nRFSN.transfer('n',FLUSH_RX,0);
  nRFSN.transfer('n',FLUSH_TX,0);
  nRFSN.clearInt(0x70);
  
  nRFSN.setRXAddr(RX_ADDR_P0, nodeAddr, 4);
}

//-------------------------------------------------------------------------------------------------
// nRFSN required code. See documentation for how to modify this for your project.


//-----------------------------------
// This code MUST stay. See documentation for user modifications.
//-----------------------------------
void nRFSN_setup() {
  nRFSN.init(SPI_CLOCK_DIV4,nRF_CE,nRF_CSN,nRF_IRQ);   // Initialize: SPI clock divider, CE pin, CSN pin, IRQ pin.
                                                       // IRQ pin must be capable of interrupts!
  
  nRFSN.setRXMode();                  // Default to RX mode
}
//-----------------------------------


//-----------------------------------
// This code MUST stay for system to work!
//-----------------------------------
void loop() {
  if ( nRFSN.updateStatus() & 0b01000000 ) {
    
    nRFSN.getPayload(4);
    
    buf = nRFSN.getBufIn(4);
    
    if (buf[0] == SENV_0) {
      analogWrite(Rpin, buf[1]);
      analogWrite(Gpin, buf[2]);
      analogWrite(Bpin, buf[3]);
    }
    
    nRFSN.transfer('n',FLUSH_RX,0);
    nRFSN.clearInt(0x70);
  }
    
}

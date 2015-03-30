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
#define nRF_CE   8    // chip enable
#define nRF_CSN  9    // chip select
#define nRF_IRQ  10    // irq

#define switchPin  7

//-----------------------------------

//-----------------------------------
// Global variables
//-----------------------------------

uint8_t nodeAddr[4] = {0xD5,0xD5,0xD5,0xD5};
uint8_t *buf = (uint8_t*)calloc(4, sizeof(uint8_t));
//-----------------------------------

//-----------------------------------
// Arduino setup loop
//-----------------------------------
void setup() {
  Serial.begin(115200);
  
  Serial.println("Start");
  
  pinMode(switchPin, OUTPUT);   
  
  nRFSN_setup();
  
  nRFSN.transfer('n',FLUSH_RX,0);
  nRFSN.transfer('n',FLUSH_TX,0);
  nRFSN.clearInt(0x70);
  
  nRFSN.setRXAddr(RX_ADDR_P0, nodeAddr, 4);
}


//-----------------------------------
// Arduino main loop
//-----------------------------------
//void loop() {
//  nRFSN_loop();    // If NOT called within 2 seconds of an nRF24L01+ event, RPi will timeout!
//}
//-----------------------------------




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
    
    nRFSN.getPayload(2);
    
    buf = nRFSN.getBufIn(2);
    
    Serial.println(buf[1]);
    
    if (buf[0] == SENV_0) {
      digitalWrite(switchPin, buf[1]);
    }
    
    nRFSN.transfer('n',FLUSH_RX,0);
    nRFSN.clearInt(0x70);
  }
    
}

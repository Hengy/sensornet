
//-----------------------------------
// Standard includes - These MUST NOT CHANGE in order for nRFSN to work!
//-----------------------------------
#include <SPI.h>
#include <EEPROM.h>
#include <nRFSN.h>
//-----------------------------------

void setup() {
  nRFSN_setup();  // This code MUST stay. See documentation for user modifications.
  
  // User setup code goes here. Make sure not to undo/change nRFSN setup!
}

void loop() {
  // put your main code here, to run repeatedly: 

}

//-----------------------------------
// This code MUST stay. See documentation for user modifications.
//-----------------------------------
void nRFSN_setup() {
  nRFSN.init(SPI_CLOCK_DIV4,5,4,3);   // Initialize: SPI clock divider, CE pin, CSN pin, IRQ pin.
                                      // IRQ pin must be capable of interrupts!
  attachInterrupt(0,nRFSN_ISR,LOW);   // Enable nRF24L01+ IRQ interrupt
}
//-----------------------------------

//-----------------------------------
// This mode MUST stay for system to work!
//-----------------------------------
void nRFSN_loop() {
  
  // If data has been received
  if (nRFSN.RXInt) {
    switch (nRFSN.BufIn[0])
    {
      case SENV_0:  // Request sensor value 0 command
      {
        byte len = 0;
        nRFSN.transmit(len);  // Transmit first sensor data
        break;
      }
      
      case AUTOV_0:  // Update automation value 0 command
      {
        
        break;
      }
      
      default:
        break;
    }
    
    nRFSN.clearInt(RX_DR);  // Clear data received interrupt
    nRFSN.RXInt = 0;
  }
  
  // If data sent
  if (nRFSN.TXInt) {
    
    nRFSN.clearInt(TX_DS); // Clear data sent interrupt
    nRFSN.TXInt = 0;
  }
  
  // If max retransmits
  if (nRFSN.MAXInt) {
    
    nRFSN.clearInt(MAX_RT); // Clear max retransmits interrupt
    nRFSN.MAXInt = 0;
  }
    
}
//-----------------------------------

//-----------------------------------
// DO NOT DELETE OR CHANGE!
//-----------------------------------
void nRFSN_ISR() {
  nRFSN.nRF_ISR();
}
//-----------------------------------

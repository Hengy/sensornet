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
#define nRF_CE   6    // chip enable

#define nRF_CSN  7    // chip select

#define nRF_IRQ  3    // irq

#define test 9

//-----------------------------------
// Global variables
//-----------------------------------
byte SENV_0_DATA1 = 0;  // sensor 1 value; used to respond to SENV_0 command
byte SENV_1_DATA1 = 0;  // sensor 2 value; used to respond to SENV_1 command

byte data[1];

byte stat, num;
//-----------------------------------


//-----------------------------------
// Arduino setup loop
//-----------------------------------
void setup() {
  Serial.begin(115600);
  
  nRFSN_setup();
  
  SENV_1_DATA1 = 0xA4;  // 164
  SENV_0_DATA1 = 0x49;  // 73

  Serial.println("Starting Node");
  Serial.print("Status(1): ");
  Serial.println(nRFSN.updateStatus(), BIN);
  
  pinMode(test, OUTPUT);
  
  nRFSN.transfer('n',FLUSH_RX,0);
  nRFSN.transfer('n',FLUSH_TX,0);
  nRFSN.clearInt(0x70);
  
  nRFSN.setTXMode();
  
  delay(2000);
}


//-----------------------------------
// Arduino main loop
//-----------------------------------
void loop() {
  
  delay(100);
  
  digitalWrite(test, HIGH);
  
  if (num == 8) {
    Serial.print("Status: ");
    Serial.println(nRFSN.updateStatus(),BIN);
    num = 0;
  }
  
  num++;
  
  nRFSN_loop();    // If NOT called within 2 seconds of an nRF24L01+ event, RPi will timeout!
}
//-----------------------------------


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
void nRFSN_loop() {
  
  nRFSN.nRF_ISR();
  
  // If data has been received
  if (nRFSN.RXInt) {
    
    char psize = nRFSN.getPayloadSize();
    nRFSN.getPayload(psize);
    
    uint8_t *buf = nRFSN.getBufIn(1);
    
    delay(2000);
    Serial.println("GO");
        
    switch (buf[0])
    {
      
      //-----------------------------------
      // Respond to commands from root here. Delete / Add necessary cases. See Documentation for default commands.
      //-----------------------------------
      case SENV_0:  // Request sensor value 0 command
      {
        // Put data to be sent in BufIn here
        data[0] = SENV_0_DATA1;
        nRFSN.putBufOut(data,1);
        nRFSN.respond(1);                                // Specify length of data (in bytes) here. Max 28 bytes
        break;
      }
      
      case SENV_1:  // Request sensor value 0 command
      {
        // Put data to be sent in BufIn here
        data[0] = SENV_1_DATA1;
        nRFSN.putBufOut(data,1);
        nRFSN.respond(1);                                // Specify length of data (in bytes) here. Max 28 bytes
        Serial.print("Status: ");
        Serial.println(nRFSN.updateStatus(),BIN);;
        break;
      }
      
      default:
        data[0] = 0xAA;
        nRFSN.putBufOut(data,1);
        nRFSN.respond(1);  // send error
        Serial.println("Err");
        delay(20);
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
//
////-----------------------------------
//// DO NOT DELETE OR CHANGE!
////-----------------------------------
//void nRFSN_ISR() {
//  
//  digitalWrite(8, HIGH);
//  
//  nRFSN.nRF_ISR();
//}
//-------------------------------------------------------------------------------------------------

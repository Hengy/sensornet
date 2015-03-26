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
//-----------------------------------

//-----------------------------------
// Global variables
//-----------------------------------
byte AUTV_0_DATA1 = 0;  // sensor 1 value; used to respond to SENV_0 command
//
unsigned long timeCount = 0;
unsigned long timeCount_prev = millis();

byte errorCount = 0;    // count the number of DHT read errors

byte data[1];

byte stat, num;
//-----------------------------------


//-----------------------------------
// Arduino setup loop
//-----------------------------------
void setup() {
  Serial.begin(115600);
  
  nRFSN_setup();
  
  Serial.println("Starting Node");
  Serial.print("Status: ");
  Serial.println(nRFSN.updateStatus(), BIN);
  
  nRFSN.transfer('n',FLUSH_RX,0);
  nRFSN.transfer('n',FLUSH_TX,0);
  nRFSN.clearInt(0x70);
}


//-----------------------------------
// Arduino reset function
//-----------------------------------
//void(* resetFunc) (void) = 0;   //declare reset function @ address 0


//-----------------------------------
// Arduino main loop
//-----------------------------------
void loop() {
  
  delay(50);

  nRFSN_loop();    // If NOT called within 2 seconds of an nRF24L01+ event, RPi will timeout!
}
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
void nRFSN_loop() {
  
  nRFSN.nRF_ISR();
  
  // If data has been received
  if (nRFSN.RXInt) {
    
    char psize = nRFSN.getPayloadSize();
    nRFSN.getPayload(psize);
    
    uint8_t *buf = nRFSN.getBufIn(2);
    
    delay(600);
    
    switch (buf[0])
    {
      
      //-----------------------------------
      // Respond to commands from root here. Delete / Add necessary cases. See Documentation for default commands.
      //-----------------------------------
      case AUTOV_0:  // Request sensor value 0 command
      {
        AUTOV_0_DATA1 = buf[1]
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

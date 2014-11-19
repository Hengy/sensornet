
//-----------------------------------
// Standard includes - These MUST NOT CHANGE in order for nRFSN to work!
//-----------------------------------
#include <SPI.h>
#include <EEPROM.h>
#include <nRFSN.h>
//-----------------------------------


//-----------------------------------
// Global variables
//-----------------------------------
byte SENV_0_DATA1 = 0;  // byte 1 of sensor 1 value; used to respond to SENV_0 command
byte SENV_0_DATA2 = 0;  // byte 2 of sensor 1 value; used to respond to SENV_0 command


void setup() {
  // User setup code goes here. Make sure not to undo/change nRFSN setup!
  // Serial setup - TESTING
  Serial.begin(115600);
  Serial.println("Running...\n");
  
  delay(1000);
  
  //nRFSN_setup();  // This code MUST stay. See documentation for user modifications.
}

void loop() {
  // Put your main code here to run repeatedly. Code should be as short as possible.
  int adval = analogRead(0);
  float temp = (25 * adval) - 2050;
  byte temp_h = (int)(temp/100);
  byte temp_l = (int)(temp-(temp_h*100));
  SENV_0_DATA1 = temp_h;
  SENV_0_DATA2 = temp_l;
  Serial.print("Temp: ");
  Serial.print(temp/100);
  Serial.write(183);
  Serial.print("C: ");
  Serial.print(temp_h);
  Serial.print(" ");
  Serial.print(temp_l);
  Serial.print("\n");
  
  delay(1000);
  
  //nRFSN_loop();  // If NOT called within 2 seconds of an nRF24L01+ event, RPi will timeout!
}




//-------------------------------------------------------------------------------------------------
// nRFSN required code. See documentation for how to modify this for your project.


//-----------------------------------
// This code MUST stay. See documentation for user modifications.
//-----------------------------------
void nRFSN_setup() {
  nRFSN.init(SPI_CLOCK_DIV4,5,4,3);   // Initialize: SPI clock divider, CE pin, CSN pin, IRQ pin.
                                      // IRQ pin must be capable of interrupts!
                                      
  Serial.println("Init complete.");
  delay(500);
                                      
  attachInterrupt(0,nRFSN_ISR,LOW);   // Enable nRF24L01+ IRQ interrupt  
  nRFSN.setRXMode();                  // Default to RX mode
  
  Serial.println("Interrup and RX mode set.");
  delay(500);
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
      
      //-----------------------------------
      // Respond to commands from root here. Delete / Add necessary cases. See Documentation for default commands.
      //-----------------------------------
      case SENV_0:  // Request sensor value 0 command
      {
        // Put data to be sent in BufIn here
        //nRFSN.respond([SENV_0_DATA1,SENV_0_DATA2],2);  // Specify length of data (in bytes) here. Max 28 bytes
        Serial.println("Responding.");
        delay(500);
        break;
      }
      
      case AUTOV_0:  // Update automation value 0 command
      {
        // Get received data length here: len = nRFSN.getPayloadSize();
        // Get received data using len here. Data is put in BufOut. Command is in BufOut[0]: nRFSN.getPayload(len,1);
        // Do something wtih payload here
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
//-------------------------------------------------------------------------------------------------

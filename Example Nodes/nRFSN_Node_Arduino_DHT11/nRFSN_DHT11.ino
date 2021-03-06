//-----------------------------------
// Standard nRFSN include - These MUST NOT CHANGE in order for nRFSN to work!
//-----------------------------------
#include <nRFSN.h>
//-----------------------------------
// Includes
//-----------------------------------
#include <SPI.h>        // SPI library
#include <EEPROM.h>     // EEPROM library
#include "DHT.h"        // DHT11 Humidity and Temp sensor library
//-----------------------------------

//-----------------------------------
// Definitions
//-----------------------------------
#define nRF_CE   6    // chip enable

#define nRF_CSN  7    // chip select

#define nRF_IRQ  3    // irq

//-----------------------------------
#define DHTPIN   4     // DHT11 data pin; 10k pullup

#define DHTTYPE DHT11  // DHT 11

//  Pin connection for DHT11
//  Connect pin 1 (on the left) of the sensor to +5V
//  Connect pin 2 of the sensor to whatever your DHTPIN is
//  Connect pin 4 (on the right) of the sensor to GROUND
//  Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor
//-----------------------------------

//-----------------------------------
// Global variables
//-----------------------------------
byte SENV_0_DATA1 = 0;  // sensor 1 value; used to respond to SENV_0 command
byte SENV_1_DATA1 = 0;  // sensor 2 value; used to respond to SENV_1 command
//
unsigned long timeCount = 0;
unsigned long timeCount_prev = millis();

//byte errorCount = 0;    // count the number of DHT read errors

byte data[1];
//-----------------------------------


//-----------------------------------
// Initialize DHT sensor
//-----------------------------------
DHT dht(DHTPIN, DHT11);
//-----------------------------------


//-----------------------------------
// Arduino setup loop
//-----------------------------------
void setup() {
//  Serial.begin(115600);
  
  nRFSN_setup();
  
  dht.begin();

//  Serial.println("Starting Node");
//  Serial.print("Status: 0b");
//  Serial.println(nRFSN.updateStatus(), BIN);
  
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
  
//  delay(100);
      
  timeCount = millis();

  if (timeCount - timeCount_prev >= 2000) {
    
    // Sensor readings may also be up to 2 seconds old
    float h = dht.readHumidity();
    SENV_1_DATA1 = nRFSN.toChar(h);
    
    // Read temperature as Celsius
    float t = dht.readTemperature();
    SENV_0_DATA1 = nRFSN.toChar(t);
    
    // Check if any reads failed and exit early (to try again).
//    if (isnan(h) || isnan(t)) {
//      Serial.println("Failed to read from DHT sensor!");
//      errorCount++;
//      if (errorCount > 5) {
//        Serial.print("RESETTING!");
//        delay(1000);
////        resetFunc();
//      }
//    }
  
    // Compute heat index
    // Must send in temp in Fahrenheit!
//    float hi = dht.computeHeatIndex(t, h);
//  
//    Serial.print("\nHumidity: "); 
//    Serial.print(h);
//    Serial.print("%\n");
//    Serial.print("Temperature: "); 
//    Serial.print(t);
//    Serial.write(183);
//    Serial.print("C\n");
//    if ((t > 27.0) && (h > 40.0)) {
//      Serial.print("Heat index: ");
//      Serial.print(hi);
//      Serial.write(183);
//      Serial.println("C");
//    } else {
//      Serial.println("Heat index invalid in these conditions");
//    }
    
    timeCount_prev = timeCount;
  } else if (timeCount_prev >= timeCount) {
    timeCount_prev = 0;
  }
  
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
    
    uint8_t *buf = (uint8_t*)calloc(1, sizeof(uint8_t));
    buf = nRFSN.getBufIn(1);
    
//    Serial.print("Status: ");
//    Serial.println(nRFSN.updateStatus(), BIN);
    
    delay(500); // for pure C root, 400
    
    switch (buf[0])
    {
      
      //-----------------------------------
      // Respond to commands from root here. Delete / Add necessary cases. See Documentation for default commands.
      //-----------------------------------
      case SENV_0:  // Request sensor value 0 command
      {
        // Put data to be sent in BufIn here
//        Serial.println("SENV_0");
//        Serial.print("Sending: ");
//        Serial.println(SENV_0_DATA1);
        data[0] = SENV_0_DATA1;
        nRFSN.putBufOut(data,1);
        nRFSN.respond(1);                                // Specify length of data (in bytes) here. Max 28 bytes
        break;
      }
      
      case SENV_1:  // Request sensor value 0 command
      {
        // Put data to be sent in BufIn here
//        Serial.println("SENV_1");
//        Serial.print("Sending: ");
//        Serial.println(SENV_1_DATA1);
        data[0] = SENV_1_DATA1;
        nRFSN.putBufOut(data,1);
        nRFSN.respond(1);                                // Specify length of data (in bytes) here. Max 28 bytes
        break;
      }
      
      default:
//        Serial.println("Responding with err");
        data[0] = 0xAA;
        nRFSN.putBufOut(data,1);
        nRFSN.respond(1);  // send error
        break;
    }    
//    Serial.print("Status: ");
//    Serial.println(nRFSN.updateStatus(), BIN);
    
    nRFSN.transfer('n',FLUSH_RX,0);
    nRFSN.transfer('n',FLUSH_TX,0);
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

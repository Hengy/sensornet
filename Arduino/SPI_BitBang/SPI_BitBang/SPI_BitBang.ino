#include <SPI.h>

const int SS_pin = 3;
const int SCK_pin = 13;
const int MISO_pin = 12;
const int MOSI_pin = 11;

byte sendValue = 0xFF;   // Value we are going to send
byte returnValue = 0;  // Where we will store the value sent by the slave

void setup()
{
  digitalWrite(SS, HIGH);  // Start with SS high
  
  // SPI setup 
  SPI.setBitOrder(MSBFIRST);              // Set most significant bit first
  SPI.setClockDivider(SPI_CLOCK_DIV128);   // Clock to Fosc/128 = 125KHz
  SPI.setDataMode(SPI_MODE1);             // Clock polarity 0; clock phase 1
  SPI.begin();                            // Start SPI
}

void loop()
{
  digitalWrite(SS_pin, LOW);        // SS low
  delayMicroseconds(10);
  returnValue = getSTATUS(); // Transmit data
  delayMicroseconds(10);
  digitalWrite(SS_pin, HIGH);       // SS high again 
}

byte getSTATUS(void) {
  SPDR = sendValue;                         // Send NOP command
  while (!(SPSR & (1<<SPIF))) {           // Wait the end of the transmission
  };
  return SPDR;                            // return the received byte
}

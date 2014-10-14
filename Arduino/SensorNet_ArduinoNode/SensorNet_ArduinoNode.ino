#include <nRFSN.h>

void setup() {
  // put your setup code here, to run once:
  nRFSN.init(SPI_CLOCK_DIV4,5,4,3,0);
}

void loop() {
  // put your main code here, to run repeatedly: 
  nRFSN.setRXMode();
}

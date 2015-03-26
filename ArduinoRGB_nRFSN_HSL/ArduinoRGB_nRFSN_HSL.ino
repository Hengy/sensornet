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
float h;

int max_rgb_val= 255;

int timeCount = 0;
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
  
  displayHSL(0.55);
  
  nRFSN.transfer('n',FLUSH_RX,0);
  nRFSN.transfer('n',FLUSH_TX,0);
  nRFSN.clearInt(0x70);
  
  uint8_t nodeAddr[4] = {0xCA,0xCA,0xCA,0xCA};
  nRFSN.setRXAddr(RX_ADDR_P0, nodeAddr, 4);
  uint8_t *RXA = (uint8_t*)calloc(4, sizeof(uint8_t));
  RXA = nRFSN.getRXAddr(RX_ADDR_P0);
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

void displayHSL(int hue) {
    if (hue > 1.0) hue = 1.0;

    // convert saturation and brightness value to decimals and init r, g, b variables
    float sat_f = 1.0;
    float bright_f = 1.0;
        
    if (hue >= 0 && hue < 120) {
			float hue_primary = 1.0 - (float(hue) / 120.0);
			float hue_secondary = float(hue) / 120.0;
			float sat_primary = 1.0 - hue_primary;
			float sat_secondary = 1.0 - hue_secondary;
                        b = 0;
			r = (bright_f * max_rgb_val) * (hue_primary + sat_primary);
			g = (bright_f * max_rgb_val) * (hue_secondary + sat_secondary);
			  
    }

    else if (hue >= 120 && hue < 240) {
			float hue_primary = 1.0 - ((float(hue)-120.0) / 120.0);
			float hue_secondary = (float(hue)-120.0) / 120.0;
			float sat_primary = 1.0 - hue_primary;
			float sat_secondary = 1.0 - hue_secondary;
			r = 0;
			g = (bright_f * max_rgb_val) * (hue_primary + sat_primary);
			b = (bright_f * max_rgb_val) * (hue_secondary + sat_secondary);
    }

    else if (hue >= 240 && hue <= 360) {
			float hue_primary = 1.0 - ((float(hue)-240.0) / 120.0);
			float hue_secondary = (float(hue)-240.0) / 120.0;
			float sat_primary = 1.0 - hue_primary;
			float sat_secondary = 1.0 - hue_secondary;
                        g = 0;
			r = (bright_f * max_rgb_val) * (hue_secondary + sat_secondary);  
			b = (bright_f * max_rgb_val) * (hue_primary + sat_primary);
    }
    
    analogWrite(Rpin, r);
    analogWrite(Gpin, g);
    analogWrite(Bpin, b);
}

//-----------------------------------
// This code MUST stay for system to work!
//-----------------------------------
void loop() {
  if ( nRFSN.updateStatus() & 0b01000000 ) {
    
    nRFSN.getPayload(3);
    
    uint8_t *buf = (uint8_t*)calloc(1, sizeof(uint8_t));
    buf = nRFSN.getBufIn(3);
    
    if (buf[0] == SENV_0) {
      h = buf[1] + buf[2];
      displayHSL(h/360);
    }
    
    nRFSN.transfer('n',FLUSH_RX,0);
    nRFSN.clearInt(0x70);
  } else {
    if (timeCount == 10) {
      h += 0.01;
      displayHSL(h);
      timeCount = 0;
    }
  }
  timeCount += 1;
    
}

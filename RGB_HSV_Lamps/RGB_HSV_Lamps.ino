// Matthew Hengeveld
// m_hengeveld@sympatico.ca

// -------------------------------------------------------------------------------------

// RGB Lamp Sketch - tx version

// Sketch for Arduino-controlled lamp, with RGB LED strips, white LED, capacitive touch,
// rotary encoder, and 434MHz RF sync to a second lamp. Includes multiple colour modes,
// as well as fade-on and fade-off effects.


// -------------------------------------------------------------------------------------
// Libraries
// -------------------------------------------------------------------------------------

#include <CapacitiveSensor.h>
#include <Encoder.h>

// -------------------------------------------------------------------------------------
// Pins
// -------------------------------------------------------------------------------------

// RGB LED pins
const byte led_b = 11; // red
const byte led_g = 10; // green
const byte led_r = 9; // blue

// White LED pin
const byte led_w = 8; // white

// rotary encoder pin
const byte rot_pinA = 3; // pin A; interrupt
const byte rot_pinB = 4;// pin B 

// select button pin
const byte sel_pin = 2; //interrupt

// capacitive sensor pins
const byte cs_in = 5; // INPUT
const byte cs_out = 6; // OUTPUT

// -------------------------------------------------------------------------------------
// Variables
// -------------------------------------------------------------------------------------


// -------------------------------------------------------------------------------------
/*
MODES and mode specific variables
0: Default/Standby - initiates offSeq()
1: "Standard" white lamp - initiates onSeq; changes colour temp/brightness on user pref
2: User selected colour - constant colour; changes temp/brightness on user pref
3: rainbow fade
4: Random colour - fades between random "standard" hues
    */
    byte nextRndHue; // next random hue index number
    int nextRndHueCount = 0; // count time colour is static
    const byte nextRndHueStatic = 200; // time colour should stay static (x100ms)
    /*
*/
byte mode = 0; // colour mode; 0 is default, and indicates standby mode (all LEDs off)
const byte numModes = 4; // number of colour modes
boolean idleMode = false; // idle mode; false = normal mode; true = idle mode
volatile boolean rotMode = false; // rotary encoder mode; false = brightness; true = hue
volatile boolean rotModeRestrict = false; // restricts rotary encoder mode; prevents change
// -------------------------------------------------------------------------------------

// timing variables
long tick; // time at beginning of loop()
unsigned int loopTime; // time it took to execute last loop

unsigned int capTime = 0; // time capacitive sensor if continuously touched

unsigned int modeTime = 0; // timing for current mode

volatile long lastInt; // time of last select button interrupt
// -------------------------------------------------------------------------------------

// timing delays/speeds
//                                            offSpeed   onSpeed    mode1   mode2   mode3
const unsigned int modeDelay[numModes + 1] = {45,        13,        0,      833,    150};
// -------------------------------------------------------------------------------------

// input thresholds
const int capThres = 300; // capacitive sensor value threshold
const byte capPressThres_low = 200; // threshold for "short" touch
const int capPressThres_high = 1250; // threshold for "long" touch
const byte selPressThres = 100; // debounce time for select button interrupt
// -------------------------------------------------------------------------------------

// colour variables - HSB model
//                       red    pink    purple   blue    turquoise   green    yellow    orange

const int standHue[8] =  {0,    320,    285,     240,    144,        120,     50,       26}; // hue
const byte standSat[8] = {255,  224,    224,     255,    255,        255,     255,      255}; // saturation

const byte redCorr = 93; // red LED correction factor percentage
// -------------------------------------------------------------------------------------

// "Neutral" white values
const byte whiteRGB[3] = {255, 255, 255}; // RGB
const int whiteHSB[3] = {0, 0, 255}; // HSB
// -------------------------------------------------------------------------------------

// current colours displayed by RGB LEDs
int currHSB[3] = {0, 0, 0}; // current colour in HSB; default is black
byte currRGB[3] = {0, 0, 0}; // current colour in RGB; determined by HSBtoRGB() function
// -------------------------------------------------------------------------------------

// capacitive sensor variables
CapacitiveSensor CapSenseObj = CapacitiveSensor(cs_in, cs_out); // capacitive sensor object
const byte capSamples = 15; // capacitive sensor samples
// -------------------------------------------------------------------------------------

// rotary encoder variables
long rotPos = -999;
long newRotPos;
Encoder RotEncObj(rot_pinA, rot_pinB); // rotary encoder object


// -------------------------------------------------------------------------------------
// setup
// -------------------------------------------------------------------------------------

void setup() {
  Serial.begin(9600); // begin serial communication
  
  // initialize RGB pins to OUTPUT
  pinMode(led_r, OUTPUT);
  pinMode(led_g, OUTPUT);  
  pinMode(led_b, OUTPUT);
  
  // turn off RGB LEDs initially
  digitalWrite(led_r, LOW);
  digitalWrite(led_g, LOW);
  digitalWrite(led_b, LOW);
  
  // initialize white pin to OUTPUT
  pinMode(led_w, OUTPUT);
  
  // turn off white led initially
  digitalWrite(led_w, LOW);
  
  // get random seed for random colour algorithms
  randomSeed(analogRead(0));
  
  // select pin interrupt
  attachInterrupt(sel_pin, selectISR, FALLING);
}


// -------------------------------------------------------------------------------------
// main loop
// -------------------------------------------------------------------------------------
void loop() {
  long capTotal = CapSenseObj.capacitiveSensor(capSamples); // detect capacitive sensor
  
  // protect against millis() overflow
  if (millis() > tick) {
    loopTime = millis() - tick;
  } else { // if millis() overflows, set times to a semi-arbitrary 30
    capTime = 30;
    modeTime = 30;
  }
  tick = millis(); // get new tick; start loopTime count
  
  // change mode based on time capacitive sensor is touched
  if (capTotal > capThres) {  // if capacitive sensor is touched
    capTime += loopTime; // count time capacitive sensor is pressed
    if (capTime > capPressThres_high) {  // if capacitive sensor is touch is "long", execute offSeq immeadiatly
      idleMode = false; // disable "idle" mode
      mode = 0; // execute offSeq function
      capTime = 0; // reset
      RotEncObj.write(0); // reset rotary encoder position to 0
      modeSet(); // set mode settings once
    } 
  } else { // when the capacitive sensor is released, check
    if (capTime < capPressThres_low) { // if capacitive sensor is touch is "short", change mode
      if (capTime > 20) { // no false readings
        idleMode = false; // disable "idle" mode
        mode++;  // next mode
        if (mode > numModes) {  // if end of modes, execute offSeq
          mode = 0; // reset to offSeq mode
        }
        RotEncObj.write(0); // reset rotary encoder position to 0
        modeSet(); // set mode settings once
      }
    }

    capTime = 0; // when capacitive touch sensor if touched, reset count
  }
   
  // change mode when capacitive sensor is touched
  if (!idleMode) { // if not in idle mode
    switch (mode) { // execute mode
      case 1: // fade RGB LEDs on if not in idle mode
        onSeq(); // turn on all LEDs      
        break;
      case 2:  // user selected colour
        mode1();
        break;
      case 3:  // rainbow fade
        modeTime += loopTime;
        checkRot(false, 10, 255); // check for brightness change
        if (modeTime > modeDelay[mode]) {
          mode2();
          modeTime = 0;
        }
        break;
      case 4:  // random "standard" colours
        modeTime += loopTime;
        checkRot(false, 10, 255); // check for brightness change
        if (modeTime > modeDelay[mode]) {
          mode3();  // random colour
          modeTime = 0;
        }
        break;
      default:  // fade RGB LEDs off is not in idle
        offSeq(); // turn off all LEDs 
    }
  } else {
    if (mode == 1) { // white mode
      changeTemp(); // check for hue change //SPECIAL CASE!!!!!!!!!
      checkRot(false, 5, 255); // check for brightness change
    } else if (mode == 2) { // user selected colour mode
      checkRot(false, 2, 255); // check for brightness change
      checkRot(true, 0, 359); // check for hue change
    }
  }
}


// -------------------------------------------------------------------------------------
// settings for specific modes; executed on mode change
// -------------------------------------------------------------------------------------
void modeSet() {
  switch (mode) {
    case 3:
      rotMode = false;
      rotModeRestrict = true;
      break;
    case 4:
      rotMode = false;
      rotModeRestrict = true;
      break;
    default:
      rotMode = false;
      rotModeRestrict = false;
  }
}

// -------------------------------------------------------------------------------------
// changes color temperature of white using RGB LEDs
// -------------------------------------------------------------------------------------
void changeTemp() {
  newRotPos = RotEncObj.read();
  if (newRotPos != rotPos) {
    
  }
}

// -------------------------------------------------------------------------------------
// change hue or brightness on rotary encoder turn
// INPUT:  boolean checkMode - determines what function checks for; false = brightness; true = hue;
//         byte bound_low - function will not set brightness/hue below this
//         byte bound_high - function will not ser brightness/hue above this
// -------------------------------------------------------------------------------------
void checkRot(boolean checkMode, byte bound_low, int bound_high) {
  newRotPos = RotEncObj.read();
  if (newRotPos != rotPos) {
    if (rotMode == checkMode) { // if the rotary mode and the mode checked for are the same
      if (checkMode) { // hue
      
        if (newRotPos > rotPos) { // if rotary encoder turned right ("increased")
          currHSB[0]++;
          if (currHSB[0] > bound_high) { // rollover
            currHSB[0] = bound_low;
          }
        } if (newRotPos < rotPos) {  // if rotary encoder turned right ("decreased")     
          if (currHSB[0] < bound_low) { // rollover
            currHSB[0] = bound_high;
          } else {
            currHSB[0]--;
          }
        }
        
      } else { // brightness
        
        if (newRotPos > rotPos) { // if rotary encoder turned right ("increased")
          if (currHSB[2] < bound_high) { // do not rollover
            currHSB[2]++;
          }
        } if (newRotPos < rotPos) {  // if rotary encoder turned right ("decreased")
          if (currHSB[2] > bound_low) { // do not go below 5 brightness
            currHSB[2]--;
          }
        }  
        
      }
    }
    HSBtoRGB();
    writeLEDs(); // display new colour
  }
}

// -------------------------------------------------------------------------------------
// converts HSB colour values to RGB
// hue: 0-359, sat: 0-255, val(B): 0-255
// http://log.liminastudio.com/itp/physical-computing/arduino-controlling-an-rgb-led-by-hue
// -------------------------------------------------------------------------------------
void HSBtoRGB() {
  int base;
  
  if (currHSB == 0) { // grey value
    currRGB[0] = currHSB[2];
    currRGB[1] = currHSB[2];
    currRGB[2] = currHSB[2];
  } else {
    base = ((255 - currHSB[1]) * currHSB[2]) >> 8; // ((255 - sat) * val(B)) / 256
    
    switch(currHSB[0]/60) {
      case 0:
        currRGB[0] = currHSB[2];
        currRGB[1] = (((currHSB[2] - base) * currHSB[0]) / 60) + base;
        currRGB[2] = base;
        break;
      case 1:
        currRGB[0] = (((currHSB[2] - base) * (60 - (currHSB[0] % 60))) / 60) + base;
        currRGB[1] = currHSB[2];
        currRGB[2] = base;
        break;
      case 2:
        currRGB[0] = base;
        currRGB[1] = currHSB[2];
        currRGB[2] = (((currHSB[2] - base) * (currHSB[0] % 60)) / 60) + base;
        break;
      case 3:
        currRGB[0] = base;
        currRGB[1] = (((currHSB[2] - base) * (60 - (currHSB[0] % 60))) / 60) + base;
        currRGB[2] = currHSB[2];
        break;
      case 4:
        currRGB[0] = (((currHSB[2] - base) * (currHSB[0] % 60)) / 60) + base;
        currRGB[1] = base;
        currRGB[2] = currHSB[2];
        break;
      case 5:
        currRGB[0] = currHSB[2];
        currRGB[1] = base;
        currRGB[2] = (((currHSB[2] - base) * (60 - (currHSB[0] % 60))) / 60) + base;
        break;
    }
  }
}

// -------------------------------------------------------------------------------------
// write current RGB values to LEDs;
// -------------------------------------------------------------------------------------
void writeLEDs() {
  analogWrite(led_r, currRGB[0]); // red
  analogWrite(led_g, currRGB[1]); // green
  analogWrite(led_b, currRGB[2]); // blue
}

// -------------------------------------------------------------------------------------
// On sequence - fades on RGB LEDs; turns on white LED; sets idle mode when complete
// -------------------------------------------------------------------------------------
void onSeq() {
  for (int i = 0; i < 255; i++) { // count to 255 (100% LED brightness)
    if (i < whiteRGB[0]) {  // if red is less than the "standard" white colour red value
      analogWrite(led_r, i); // write i brightness value
    } else { 
      analogWrite(led_r, whiteRGB[0]); // else write "Standard" white red value
    }
    // write green and blue values
    analogWrite(led_g, i);
    analogWrite(led_b, i);
    
    if (i == 215) { // if i is equal to 85%
      digitalWrite(led_w, HIGH); // turn on top LED
    }
    
    delay(modeDelay[1]); // delay for fade on effect
  }
  
  digitalWrite(led_w, HIGH); // make sure top LED is on
  
  // make sure LEDs are at white colour
  analogWrite(led_r, whiteRGB[0]);
  analogWrite(led_g, whiteRGB[1]);
  analogWrite(led_b, whiteRGB[2]);
  
  // update current colour
  currRGB[0] = whiteRGB[0];
  currRGB[1] = whiteRGB[1];
  currRGB[2] = whiteRGB[2];
  
  currHSB[0] = whiteHSB[0];
  currHSB[1] = whiteHSB[1];
  currHSB[2] = whiteHSB[2];
  
  idleMode = true; // when finished, go into idle mode
}

// -------------------------------------------------------------------------------------
// user selected colour
// -------------------------------------------------------------------------------------
void mode1() {
  // set colour to red
  currHSB[0] = 0;
  currHSB[1] = 255;
  currHSB[2] = 255;
  
  // convert and write to LEDs
  HSBtoRGB();
  writeLEDs();
  
  idleMode = true; // set idle mode
}

// -------------------------------------------------------------------------------------
// rainbow
// -------------------------------------------------------------------------------------
void mode2() {
  currHSB[0]++;
  if (currHSB[0] > 359) {
    currHSB[0] = 0;
  }
  
  // convert and write to LEDs
  HSBtoRGB();
  writeLEDs();
}

// -------------------------------------------------------------------------------------
// random fade
// -------------------------------------------------------------------------------------
void mode3() {
  nextRndHueCount++;
  int prev = nextRndHue; // previous colour
  if (nextRndHueCount >= nextRndHueStatic) {
    int high = max(currHSB[0], nextRndHue);
    int low = min(currHSB[0], nextRndHue);
    if ((high - low) > 180) { // take shortest route to next colour
      currHSB[0]--;
    } else if ((high - low) == 0) { // if current colour equals next colour, finish fade
      nextRndHueCount = 0;
      while (nextRndHue == prev) { // do not have to same colours in a row
        nextRndHue = standHue[random(8)]; // choose new colour from standard hues
      }
    } else {
      currHSB[0]++;
    }
    RGBtoHSB();
    writeLEDs();
  }
}

// -------------------------------------------------------------------------------------
// Off sequence - fades RGB LEDs until HSB brightness is zero; turns off white LED;
//                sets idle mode when complete
// -------------------------------------------------------------------------------------
void offSeq() {
  digitalWrite(led_w, LOW); // turn off white LEDs
  
  while (currHSB[2] > 0) { // while HSB brightness value is above 0
    currHSB[2]--; // decrement brightness
    
    HSBtoRGB(); // convert to RGB values
    writeLEDs(); // display new colour
    
    delay(modeDelay[0]); // delay for fade off effect
  }
  
  // update current colour when complete
  currRGB[0] = 0;
  currRGB[1] = 0;
  currRGB[2] = 0;
  
  currHSB[0] = 0;
  currHSB[1] = 0;
  currHSB[2] = 0;
  
  idleMode = true; // when finished, go into idle mode
  
  randomSeed(analogRead(0)); // generate random seed for next mode
}

// -------------------------------------------------------------------------------------
// Interrupt Service Routines
// -------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------
// ISR pin 2; FALLING
// toggles rotary encoder mode between brightness and hue
// -------------------------------------------------------------------------------------
void selectISR() {
  if (!rotModeRestrict) { // if there is no restriction on the rotary encoder mode
    if ((millis() - lastInt) >= selPressThres) { // if the interrupt hasn't executed within 100ms
      rotMode = !rotMode; // change what turning rotary knob does
      RotEncObj.write(0); // reset rotary encoder position to 0
      lastInt = millis(); // if interrupt executes, reset to 0
    }
  }
}

#include <FastLED.h>
#include <DMXSerial.h>

#include "masterDefs.h"
#include "ledFuncs.h"
#include "screenFuncs.h"

uint8_t mode = 0; //default mode is pixel mapped mode
//MODE DEFINITIONS
//MODE 1: Pixel Mapped (150 channels)
//MODE 2: RGB, FX select and FX speed (5 channels)

int address = 1; //default address is 1 (can this be changed to not change when device powers off)

CRGB leds[NUM_LEDS]; //defines LED object

#define SL 0 //defines starting LED
#define EL NUM_LEDS - 1 //defines ending LED
#define WAIT_TIME (-1.0 / 25.0 * NUM_LEDS) + 7

bool pcintg = true;

// MASTER FX LIST for 5 channel mode
// DMX VALUES for CHANNEL 4 when in 5 Channel Mode
// CHANNELS:
// 0 ----- Solid Color
// 1<49 -- Strobe (speed fast to slow)
// 50<59 front to back 60<69 back to front 70<79 mirror out 80<89 mirror in (runner effects)

void FXSel(int f) { //functions used for FX selection
  if (f == 0 || f == 255)
    solidRGB(leds, DMXSerial.read(address),
             DMXSerial.read(address + 1),
             DMXSerial.read(address + 2));
  else if (f > 0 && f < 50)
    strobe(leds,
      DMXSerial.read(address),
      DMXSerial.read(address + 1),
      DMXSerial.read(address + 2),
      2 * DMXSerial.read(address + 4));
  else if (f >= 50 && f < 90)
    runner(leds, DMXSerial.read(address),
           DMXSerial.read(address + 1),
           DMXSerial.read(address + 2),
           ((DMXSerial.read(address + 3) - 50) / 10) + 1);
}

void pixelMap() {
  for (int i = SL; i <= EL; i++) {
    leds[i] = CRGB(DMXSerial.read(address + 2 + (3 * i)),
                   DMXSerial.read(address + (3 * i)),
                   DMXSerial.read(address + 1 + (3 * i)));
  }
  FastLED.show();
  delay(10); //more delay needed for pixel map mode
}

//screen functions
void mainDisp(){
  ;
}

void errorDisp(){
  ;
}

void modeDisp(){
  ;
}

//button functions to be called by interrupts
void modeSelect(){
    if(mode==1)
        mode = 0;
    else
        mode++;
}

void addressUp(){
    if(address==512)
        address = 1;
    else
        address++;
}

void addressDn(){
    if(address==1)
        address = 512;
    else
        address--;
}

//functions for the LED output based on modes
void pixMap(){ //LED output based on DMX channel
    ;
}

void fiveChannel(){
    ;
}

//Interrupts
ISR(INT0_vect){ //pin 2
  addressUp();
}

ISR(INT1_vect){ //pin 3
  modeSelect();
}

ISR(PCINT0_vect){ //pin 8
  if(pcintg){
    addressDn();
    pcintg=false;
  }
  else
    pcintg=true;
  //need true and false statements to only run this based on one change instead of two changes (button push and release)
}

void setup() {
  FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);  // BRG ordering is typical
  DMXSerial.init(DMXReceiver);

  pinMode(A7, INPUT);
  blackout(leds);
  randomSeed(analogRead(7));
}

void loop() {
    if (mode==0)
        pixMap();
    else if(mode==1)
        fiveChannel();
    else
        errorDisp();
}
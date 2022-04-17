//add "__AVR_ATmega328P__", to config file to define registers
#include <FastLED.h>
#include <DMXSerial.h>
#include <Wire.h>
#include <SparkFun_Alphanumeric_Display.h>

#include "masterDefs.h"
#include "ledFuncs.h"
#include "screenFuncs.h"

int mode = 0; //default mode is pixel mapped mode
//MODE DEFINITIONS
//MODE 1: Pixel Mapped (150 channels)
//MODE 2: RGB, FX select and FX speed (5 channels)

int address = 1; //default address is 1 (can this be changed to not change when device powers off)

CRGB leds[NUM_LEDS]; //defines LED object

bool pcintg = true;

HT16K33 display; //define display setup

// MASTER FX LIST for 5 channel mode
// DMX VALUES for CHANNEL 4 when in 5 Channel Mode
// CHANNELS:
// 0 ----- Solid Color
// 1<49 -- Strobe (speed fast to slow)
// 50<59 front to back 60<69 back to front 70<79 mirror out 80<89 mirror in (runner effects)

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
  mainDisp(display, address);
}

void addressDn(){
  if(address==1)
      address = 512;
  else
      address--;
  mainDisp(display, address);
}

//functions for the LED output based on modes
void pixMap() {
  for (int i = SL; i <= EL; i++) {
    leds[i] = CRGB(DMXSerial.read(address + (3 * i)), //RED
                  DMXSerial.read(address + 1 + (3 * i)), //GREEN
                  DMXSerial.read(address + 2 + (3 * i))); //BLUE
  }
  FastLED.show();
  delay(10); //more delay needed for pixel map mode
}

void fiveChannel() { //functions used for FX selection
  int f = DMXSerial.read(address + 3);
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

//a debugging function that turns lamp on and off based on certain input
void lamp(int reps){
  for(int i = 0; i<reps; i++){
    digitalWrite(DBLED_PIN, HIGH);
    delay(500);
    digitalWrite(DBLED_PIN, LOW);
    delay(500);
  }
}

//Interrupts
ISR(INT0_vect){ //pin D2 (INT0) //works as of 4/7
  addressUp();
}

ISR(INT1_vect){ //pin D3 (INT1) //works as of 4/7
  modeSelect();
  lamp(3);
}

ISR(PCINT0_vect){ //pin D8 (PCINT0/PB0) //works as of 4/7
  if(pcintg){
    addressDn();
    pcintg=false;
  }
  else
    pcintg=true;
  //need true and false statements to only run this based on one change instead of two changes (button push and release)
}

void setup() {
  //setup interrupts
  //EIMSK |= (1<<INT0);
/*  EICRA |= (1<<ISC01); //set to trigger on falling voltage

  EIMSK |= (1<<INT1); //turn on INT1
  EICRA |= (1<<ISC11); //set to trigger on falling voltage

  PCICR |= (1<<PCIE0); //turn on PCINT0
  PCMSK0 |= (1<<PCINT0); //mask to use pin PB0 which is D8

  sei(); //enable global interrupts
*/
  FastLED.addLeds<WS2811, DATA_PIN, BRG>(leds, NUM_LEDS);  // BRG ordering is typical
  DMXSerial.init(DMXReceiver);
  
  pinMode(ADRUP_PIN, INPUT);
  pinMode(ADRDN_PIN, INPUT);
  pinMode(MODE_PIN, INPUT);
  pinMode(A7, INPUT);
  pinMode(DBLED_PIN, OUTPUT);
  blackout(leds);
  randomSeed(analogRead(7));

  Wire.begin(); //Join I2C bus
  //check if displays will acknowledge
  display.begin(0x70);

  //init DB LED
  digitalWrite(DBLED_PIN, LOW);

  startDisp(display);
  delay(1000);
  mainDisp(display, address);
  delay(1000);
  modeDisp(display, mode);
  delay(1000);

}

void loop() {
  // DMXRXDisp(display, DMXSerial.read(address));
  // solidHSV(leds, DMXSerial.read(address), 255, 255);
  // delay(30);
  pixMap();
  modeDisp(display, mode);
  /*if (mode==0)
    pixMap();
  else if(mode==1)
    fiveChannel();
  else
    errorDisp(display);*/
  delay(30);
}
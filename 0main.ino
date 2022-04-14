//add "__AVR_ATmega328P__", to config file to define registers
#include <FastLED.h>
#include <DMXSerial.h>
#include <Wire.h>
#include <SparkFun_Alphanumeric_Display.h>

#include "masterDefs.h"
#include "ledFuncs.h"
#include "screenFuncs.h"

char numMap[10] = {'0','1','2','3','4','5','6','7','8','9'};

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

//bool dbstate = 0;

HT16K33 display; //define display setup

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
/*
void pixelMap() {
  for (int i = SL; i <= EL; i++) {
    leds[i] = CRGB(DMXSerial.read(address + 2 + (3 * i)),
                   DMXSerial.read(address + (3 * i)),
                   DMXSerial.read(address + 1 + (3 * i)));
  }
  FastLED.show();
  delay(10); //more delay needed for pixel map mode
}
*/
//screen functions
void mainDisp(){
  display.clear();
  display.printChar('A',0);
  display.printChar(numMap[address/100],1);
  display.printChar(numMap[(address/10)%10],2);
  display.printChar(numMap[address%10],3);
  display.updateDisplay();
}

void errorDisp(){
  ;
}

void modeDisp(){
  ;
}

void startDisp(){
  display.clear();
  display.printChar('D',0);
  display.printChar('M',1);
  display.printChar('X',2);
  display.updateDisplay();
}

void rxDisp(){
  display.clear();
  display.printChar('R',0);
  display.printChar('X',1);
  display.updateDisplay();
}

void DMXRXDisp(int value){
  display.clear();
  display.printChar('V',0);
  display.printChar(numMap[value/100],1);
  display.printChar(numMap[(value/10)%10],2);
  display.printChar(numMap[value%10],3);
  display.updateDisplay();
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
    mainDisp();
}

void addressDn(){
    if(address==1)
        address = 512;
    else
        address--;
    mainDisp();
}

//functions for the LED output based on modes
void pixMap(){ //LED output based on DMX channel
    ;
}

void fiveChannel(){
    ;
}

void lamp(){ //a debugging function that turns lamp on and off based on certain input
  digitalWrite(DBLED_PIN, HIGH);
  delay(500);
  digitalWrite(DBLED_PIN, LOW);
  delay(500);
}

//Interrupts
ISR(INT0_vect){ //pin D2 (INT0) //works as of 4/7
  addressUp();
  //lamp();
  //lamp();
}

ISR(INT1_vect){ //pin D3 (INT1) //works as of 4/7
  modeSelect();
  //lamp();
  //lamp();
}

ISR(PCINT0_vect){ //pin D8 (PCINT0/PB0) //works as of 4/7
  if(pcintg){
    addressDn();
    lamp();
    lamp();
    pcintg=false;
  }
  else
    pcintg=true;
  //need true and false statements to only run this based on one change instead of two changes (button push and release)
}

void setup() {
  //setup interrupts
  //EIMSK |= (1<<INT0);
  EICRA |= (1<<ISC01); //set to trigger on falling voltage

  EIMSK |= (1<<INT1); //turn on INT1
  EICRA |= (1<<ISC11); //set to trigger on falling voltage

  PCICR |= (1<<PCIE0); //turn on PCINT0
  PCMSK0 |= (1<<PCINT0); //mask to use pin PB0 which is D8

  sei(); //enable global interrupts

  FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);  // BRG ordering is typical
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

  startDisp();
  delay(1000);
  mainDisp();
}

void loop() {
  FXSel(DMXSerial.read(address + 3));
  //DMXRXDisp(DMXSerial.read(address));
  //solidHSV(leds, DMXSerial.read(address), 255, 255);
  delay(30);
/*
    if (mode==0)
        pixMap();
    else if(mode==1)
        fiveChannel();
    else
        errorDisp();
        */
}
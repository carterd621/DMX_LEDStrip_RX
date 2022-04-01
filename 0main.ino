#include <FastLED.h>
#include <DMXSerial.h>

#include "masterDefs.h"
#include "ledFuncs.h"

uint8_t mode = 0; //default mode is pixel mapped mode
//MODE DEFINITIONS
//MODE 1: Pixel Mapped (150 channels)
//MODE 2: RGB, FX select and FX speed (5 channels)

int address = 1; //default address is 1 (can this be changed to not change when device powers off)

CRGB leds[NUM_LEDS]; //defines LED object

#define SL 0 //defines starting LED
#define EL NUM_LEDS - 1 //defines ending LED
#define WAIT_TIME (-1.0 / 25.0 * NUM_LEDS) + 7

//////////////////EFFECTS BUILDERS///////////////////

void fadeOut(int t, int h, int s, int v) {
  int vc = v;  //define brightness changing as the LEDs dim
  while (vc > 0) {
    vc--;
    for (int j = SL; j <= EL; j++)
      leds[j] = CHSV(h, s, vc);
    FastLED.show();
    delay(double(t / v));
  }
}

void fadeIn(int t, int h, int s, int v) { //fades to the RGB level across all inciated LEDs
  int vc = 0;
  while (vc < v) {
    vc++;
    for (int i = SL; i <= EL; i++)
      leds[i] = CHSV(h, s, vc);
    FastLED.show();
    delay(double(t / v));
  }
}

void fadeColor(double t, double h0, double h, int sat, int bright) { //fades out from RGB across indicated LEDs
  double hstep = (h - h0) / (t / 10);
  for (int j = SL; j <= EL; j++) {
    leds[j] = CHSV(h0, sat, bright);
  }
  FastLED.show();
  for (int i = 0; i < t; i += 10) {
    h0 = h0 + hstep;
    for (int k = SL; k < EL; k++)
      leds[k] = CHSV(h0, sat, bright);
    FastLED.show();
    delay(10);
  }
}

void runner(int r, int g, int b, int mode) { //mode1: forward //mode2: backward //mode3: mirror out //mode4: mirror in
  if (mode == 1) {
    for (int i = SL; i <= EL; i++) {
      leds[i] = CRGB(b, r, g);
      FastLED.show();
      delay(2 * WAIT_TIME);
    }
  }
  else if (mode == 2) {
    for (int i = EL; i >= SL; i--) {
      leds[i] = CRGB(b, r, g);
      FastLED.show();
      delay(2 * WAIT_TIME);
    }
  }
  else if (mode == 3) {
    for (int i = (EL - SL) / 2; i >= SL; i--) {
      leds[i] = CRGB(b, r, g);
      leds[EL - i] = CRGB(b, r, g);
      FastLED.show();
      delay(5 * WAIT_TIME);
    }
  }
  else if (mode == 4) {
    for (int i = SL; i <= ((EL - SL) / 2 + 1); i++) {
      leds[i] = CRGB(b, r, g);
      leds[EL - i] = CRGB(b, r, g);
      FastLED.show();
      delay(5 * WAIT_TIME);
    }
  }
  blackout(leds);
}

void strobe() { //change strobe to be specific
  int r = DMXSerial.read(address);
  int g = DMXSerial.read(address + 1);
  int b = DMXSerial.read(address + 2);
  int t = 2 * DMXSerial.read(address + 4);
  //  for (int k = 0; k < 10; k++) {
  for (int i = SL; i <= EL; i++)
    leds[i] = CRGB(b, r, g);
  FastLED.show();
  delay(6);
  for (int i = SL; i < EL; i++)
    leds[i] = CRGB(0, 0, 0);
  FastLED.show();
  delay(t);
  //  }
}

void blnk(int t) {
  leds[0] = CRGB(0, 100, 0);
  FastLED.show();
  delay(t);
  leds[0] = CRGB(0, 0, 0);
  FastLED.show();
  delay(t);
}

//////////////////EFFECTS///////////////////
void solidRGB(int r, int g, int b) {
  for (int i = SL; i <= EL; i++) {
    leds[i] = CRGB(b, r, g); //BRG ordering
  }
  FastLED.show();
}

void solidHSV(int h, int s, int v) {
  for (int i = SL; i <= EL; i++) {
    leds[i] = CHSV(h, s, v);
  }
  FastLED.show();
}

void dj() {
  int mode = random(0, 1000);
  if (mode <= 150)
    strobe();
  else if ((mode <= 400) && (mode > 150)) {
    int color1 = random(0, 13) * 10;
    int color2 = random(14, 26) * 10;
    int tFade = random(300, 1500);
    fadeColor(tFade, color1, color2, 255, 255);
  }
  else if ((mode <= 700) && (mode > 400)) {
    int color = random(0, 26) * 10;
    solidHSV(color, 0, 150);
    delay(random(0, 5) * 100);
  }
  else if (mode > 700) {
    if (rand() % 2 == 0)blackout(leds);
    runner(random(0, 245), 255, 200, random(100, 500) / 100);
  }
}

// MASTER FX LIST for 5 channel mode
// DMX VALUES for CHANNEL 4 when in 5 Channel Mode
// CHANNELS:
// 0 ----- Solid Color
// 1<49 -- Strobe (speed fast to slow)
// 50<59 front to back 60<69 back to front 70<79 mirror out 80<89 mirror in (runner effects)

void FXSel(int f) { //functions used for FX selection
  if (f == 0 || f == 255)
    solidRGB(DMXSerial.read(address),
             DMXSerial.read(address + 1),
             DMXSerial.read(address + 2));
  else if (f > 0 && f < 50)
    strobe();
  else if (f >= 50 && f < 90)
    runner(DMXSerial.read(address),
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
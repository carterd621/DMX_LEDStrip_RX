#include "masterDefs.h"

void blackout(CRGB leds[NUM_LEDS]) {
  for (uint8_t i = SL; i <= EL; i++) {
    leds[i] = CRGB::Black;
  }
  FastLED.show();
}

void solidRGB(CRGB leds[NUM_LEDS], int r, int g, int b) {
  for (int i = SL; i <= EL; i++) {
    leds[i] = CRGB(r, g, b);
  }
  FastLED.show();
}

void solidHSV(CRGB leds[NUM_LEDS], int h, int s, int v) {
  for (int i = SL; i <= EL; i++) {
    leds[i] = CHSV(h, s, v);
  }
  FastLED.show();
}

void fadeOut(CRGB leds[NUM_LEDS], int t, int h, int s, int v) {
  int vc = v;  //define brightness changing as the LEDs dim
  while (vc > 0) {
    vc--;
    for (int j = SL; j <= EL; j++)
      leds[j] = CHSV(h, s, vc);
    FastLED.show();
    delay(double(t / v));
  }
}

void fadeIn(CRGB leds[NUM_LEDS], int t, int h, int s, int v) { //fades to the RGB level across all inciated LEDs
  int vc = 0;
  while (vc < v) {
    vc++;
    for (int i = SL; i <= EL; i++)
      leds[i] = CHSV(h, s, vc);
    FastLED.show();
    delay(double(t / v));
  }
}

void fadeColor(CRGB leds[NUM_LEDS], double t, double h0, double h, int sat, int bright) { //fades out from RGB across indicated LEDs
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

void runner(CRGB leds[NUM_LEDS], int r, int g, int b, int mode) { //mode1: forward //mode2: backward //mode3: mirror out //mode4: mirror in
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

void strobe(CRGB leds[NUM_LEDS], int r, int g, int b, int t) { //change strobe to be specific
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

void blnk(CRGB leds[NUM_LEDS], int t) {
  leds[0] = CRGB(0, 100, 0);
  FastLED.show();
  delay(t);
  leds[0] = CRGB(0, 0, 0);
  FastLED.show();
  delay(t);
}
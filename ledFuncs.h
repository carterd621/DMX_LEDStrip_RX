#include "masterDefs.h"

void blackout(CRGB leds[NUM_LEDS]) {
  for (uint8_t i = SL; i <= EL; i++) {
    leds[i] = CRGB::Black;
  }
  FastLED.show();
}
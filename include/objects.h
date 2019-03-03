#pragma objects_h

#include <GyverButton.h>
GButton touch(BTN_PIN, LOW_PULL, NORM_OPEN);

#include <FastLED.h>
CRGB leds[NUM_LEDS];
CRGBPalette16 gPal;

#include <GyverTimer.h>
GTimer_ms effectTimer(60);
GTimer_ms whiteTimer(60);
GTimer_ms brightTimer(20);

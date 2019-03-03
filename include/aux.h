#pragma aux_h
#include "global_scope.h"

void lightOne(byte j, CRGB newcolor) {
  FastLED.clear();
  leds[j] = newcolor;
}

void fillAll(CRGB newcolor) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = newcolor;
  }
}

void setPixel(int Pixel, byte red, byte green, byte blue) {
  leds[Pixel].r = red;
  leds[Pixel].g = green;
  leds[Pixel].b = blue;
}

void setAll(byte red, byte green, byte blue) {
  for(int i = 0; i < NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue);
  }
}

void generateSetOfNumbers(int arr[], int n) {
  int *p;
  p = new int [NUM_LEDS];
  for (int i = 0; i < NUM_LEDS; ++i) p[i] = i;
  for (int i = NUM_LEDS; i > 0; --i) {
    int j = random(i);
    int temp = p[i];
    p[i] = p[j];
    p[j] = temp;
  }
  for (int i = 0; i < n; ++i)
    arr[i] = p[i];
  delete[] p;
}


void setPixelHeatColor (int Pixel, byte temperature) {
  // Scale 'heat' down from 0-255 to 0-191
  byte t192 = round((temperature/255.0)*191);
  // calculate ramp up from
  byte heatramp = t192 & 0x3F; // 0..63
  heatramp <<= 2; // scale up to 0..252
  // figure out which third of the spectrum we're in:
  if( t192 > 0x80) {                     // hottest
    setPixel(Pixel, 255, 255, heatramp);
  } else if( t192 > 0x40 ) {             // middle
    setPixel(Pixel, 255, heatramp, 0);
  } else {                               // coolest
    setPixel(Pixel, heatramp, 0, 0);
  }
}

/*
  - 1 x tap - on/off
  - 2 x tap - switch motion mode
  - 3 x tap - switch full color mode
*/

#include <stdint.h>

#include "objects.h"
#include "effects.h"
#include "logistics.h"

void respondToTap();

void setup() {
  Serial.begin(9600);
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  if (CURRENT_LIMIT > 0) FastLED.setMaxPowerInVoltsAndMilliamps(5, CURRENT_LIMIT / NUM_STRIPS);
  FastLED.setBrightness(brightness);
  FastLED.show();

  randomSeed(analogRead(0));
  touch.setTimeout(300);
  touch.setStepTimeout(50);
}

void loop() {
  touch.tick();
  respondToTap();
  brightnessTick();
}

void respondToTap() {
  if (touch.hasClicks()) {
    byte clicks = touch.getClicks();
    switch (clicks) {
      case 1:
        powerDirection = !powerDirection;
        powerActive = true;
        tempBrightness = brightness * !powerDirection;
        break;
      case 2:
        if (!whiteMode && !powerActive) {
          nextMotionMode();
        }
        if (whiteMode && !powerActive) {
          nextWhiteMode();
        }
        break;
      case 3:
        if (!powerActive) {
          whiteMode = !whiteMode;
          if (whiteMode) {
            effectTimer.stop();
            whiteTimer.start();
          } else {
            whiteTimer.stop();
            effectTimer.start();
          }
        }
        break;
      default:
        break;
    }
  }
  if (touch.isStep()) {
    if (!powerActive) {
      wasStep = true;
      if (brightDirection) {
        brightness += 5;
      } else {
        brightness -= 5;
      }
      brightness = constrain(brightness, MIN_BRIGHTNESS, 255);
      FastLED.setBrightness(brightness);
      FastLED.show();
    }
  }
  if (touch.isRelease()) {
    if (wasStep) {
      wasStep = false;
      brightDirection = !brightDirection;
    }
  }
  if (effectTimer.isReady() && powerState && !whiteMode) {
    switch (thisMotionMode) {
      case 0:
        makeItRain(0xff, 0xff, 0xff, true);
        break;
      case 1:
        Fire(40, 150);
        break;
      case 2:
        meteorRain(0xff, 0xff, 0xff, 1, 100, true);
        break;
      case 3:
        bouncingBall();
        break;
      case 4:
        Sparkle();
        break;
    }
    FastLED.show();
  }
  if (whiteTimer.isReady() && powerState && whiteMode) {
    switch (thisWhiteMode) {
      case 0:
        simpleWhite();
        break;
      case 1:
        tungstenWhite();
        break;
      case 2:
        hueWheel();
        break;
      case 3:
        rainbow();
        break;
      case 4:
        armenia();
        break;
    }
    FastLED.show();
  }
}

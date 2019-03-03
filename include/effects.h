#pragma effects_h
#include "aux.h"

#define TRACK_STEP 40

// ----------------- UP/DOWN -----------------
int8_t position;
bool direction;

void updown() {
  FastLED.clear();
  if (direction) {
    position++;
    if (position > NUM_LEDS - 2) {
      direction = false;
    }
  } else {
    position--;
    if (position < 1) {
      direction = true;
    }
  }
  leds[position] = CRGB::White;
}

// ----------------- SPARKLE -----------------
void Sparkle() {
  int npxs = random(NUM_LEDS) + 1;
  int *pixels = new int [npxs];
  generateSetOfNumbers(pixels, npxs);
  FastLED.clear();
  uint8_t gHue = random(0, 360);
  uint8_t gHueDelta = random(5, 360/15);
  for (int i = 0; i < npxs; ++i) {
    gHue += gHueDelta;
    gHue = gHue % 360;
    leds[pixels[i]] = CHSV(gHue, 255, 255);
  }
  delete [] pixels;
}

// ----------------- FIRE -----------------
void Fire(int Cooling, int Sparking) {
  static byte heat[NUM_LEDS];
  int cooldown;
  // Step 1.  Cool down every cell a little
  for( int i = 0; i < NUM_LEDS; i++) {
    cooldown = random(0, ((Cooling * 10) / NUM_LEDS) + 2);

    if(cooldown>heat[i]) {
      heat[i]=0;
    } else {
      heat[i]=heat[i]-cooldown;
    }
  }
  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for( int k= NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' near the bottom
  if( random(255) < Sparking ) {
    int y = random(7);
    heat[y] = heat[y] + random(160,255);
    //heat[y] = random(160,255);
  }
  // Step 4.  Convert heat to LED colors
  for( int j = 0; j < NUM_LEDS; j++) {
    setPixelHeatColor(j, heat[j] );
  }
}

// ----------------- METEOR -----------------
int meteorCntr1 = random(NUM_LEDS * 2), nextMeteor1 = -random(NUM_LEDS * 3);
int meteorCntr2 = random(NUM_LEDS * 2), nextMeteor2 = -random(NUM_LEDS * 3);
void meteorRain(byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay) {
  // fade brightness all LEDs one step
  for (int j = 0; j < NUM_LEDS; ++j) {
    leds[j].fadeToBlackBy(meteorTrailDecay);
    if((!meteorRandomDecay) || (random(10) > 5)) {
      leds[j].fadeToBlackBy(meteorTrailDecay);
    }
  }
  // draw meteor
  for (int j = 0; j < meteorSize; j++) {
    if((meteorCntr1 - j < NUM_LEDS) && (meteorCntr1 - j >= 0)) {
      setPixel(meteorCntr1 - j, red, green, blue);
    }
    if((meteorCntr2 - j < NUM_LEDS) && (meteorCntr2 - j >= 0)) {
      setPixel(meteorCntr2 - j, red, green, blue);
    }
  }
  FastLED.show();
  meteorCntr1 --;
  meteorCntr2 --;
  if (meteorCntr1 < nextMeteor1) {
    nextMeteor1 = -random(NUM_LEDS * 3);
    meteorCntr1 = NUM_LEDS - 1;
  }
  if (meteorCntr2 < nextMeteor2) {
    nextMeteor2 = -random(NUM_LEDS * 3);
    meteorCntr2 = NUM_LEDS - 1;
  }
}

// ----------------- RAIN -----------------
float g = 0.02;
class raindrop {
public:
  float h;
  float v;
  int sat;
  int val;
  int hue;
  raindrop() {
    reset();
  }
  void step() {
    v += g;
    h -= v;
    if (h < 0) {
      reset();
    }
  }
  int getH() {
    return (int)h;
  }
  void reset() {
    v = 0;
    h = random(1.5 * NUM_LEDS, 4 * NUM_LEDS);
    sat = random(0, 150);
    val = random(0, 255);
    hue = random(-20, 20);
  }
};
void makeThunder();
void drawRain();
raindrop myDrops[20];
int thunder_cntr = 1000, n_thunders = 0;
int thunder_threshold = 10;
void makeItRain(byte red, byte green, byte blue, bool thunder) {
  if (thunder) {
    thunder_cntr ++;
    if ((n_thunders > 0) && (thunder_cntr > random(2, 5))) {
      thunder_cntr = 0;
      n_thunders --;
      makeThunder();
    }
    if ((n_thunders == 0) && (random(0, 500) <= 2)) {
      n_thunders = random(0, 3);
      thunder_cntr = 0;
      makeThunder();
      return;
    }
    for (int i = NUM_LEDS - 1; i >= 0; --i) {
      if (thunder_cntr < thunder_threshold) leds[i].fadeToBlackBy(50);
      else leds[i].fadeToBlackBy(min(thunder_cntr + 100, 220));
    }
    drawRain();
  } else {
    FastLED.clear();
    drawRain();
  }
}
void drawRain() {
  for (int i = 0; i < 20; ++i) {
    myDrops[i].step();
    int hh = myDrops[i].getH();
    if ((hh >= 0) && (hh < NUM_LEDS))
      leds[hh] = CHSV(140 + myDrops[i].hue, myDrops[i].sat, myDrops[i].val);
  }
}
void makeThunder() {
  for (int i = NUM_LEDS - 1; i >= 0; --i) {
    if (random(0, 10) >= 6) {
      leds[i] = 0x3f5781;
    } else {
      leds[i] = 0xFFFCFF;
    }
    FastLED.show();
    delay(5);
  }
}

// ----------------- bouncing ball -----------------
float new_g = 0.15;
float damping = 0.2;
class ball {
public:
  float h, h_prev;
  float v;
  int hue;
  ball() : h(random(NUM_LEDS)), v(0), hue(random(256)) {}
  void step() {
    if ((h <= 0.1) && (v < 0.1)) {
      if (random(10) > 6) v = random(24) / 6.;
      hue += random(10, 30);
    }
    h_prev = h;
    v -= new_g;
    h += v;
    if (((int)(h_prev) != (int)h) && (int)h == 0) hue -= random(10, 30);
    if ((h < 0) && (v < 0)) {
      v = -v * (1 - damping);
      h = 0;
      return;
    }
    if (h >= NUM_LEDS - 1) {
      v = -v;
      return;
    }
  }
  int getH() {
    return constrain((int)h, 0, NUM_LEDS - 1);
  }
};
ball myBall;
void bouncingBall() {
  FastLED.clear();
  int hh = myBall.getH();
  leds[hh] = CHSV(myBall.hue, 255, 255);
  myBall.step();
}

// ----------------- WHITE -----------------
void simpleWhite() {
  setAll(255, 255, 255);
}
void tungstenWhite() {
  setAll(255, 147, 41);
}
void armenia() {
  for (int i = 0; i < NUM_LEDS; ++i) {
    if (i < 5) leds[i] = 0xF2A800;
    else if (i < 10) leds[i] = 0x0033A0;
    else leds[i] = 0xD90012;
  }
}

// ----------------- HUE WHEEL -----------------
float hue;
void hueWheel() {
  hue += 0.2;
  CRGB thisColor = CHSV((int)hue, 255, 255);
  fillAll(CHSV(hue, 255, 255));
}

// ----------------- RAINBOW -----------------
void rainbow() {
  hue += 2;
  for (byte i = 0; i < NUM_LEDS; i++)
    leds[i] = CHSV((byte)(hue + i * float(255 / NUM_LEDS)), 255, 255);
}

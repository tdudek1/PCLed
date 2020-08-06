#include <FastLED.h>

#define BRIGHTNESS 255
#define STRIPCOUNT 15
#define FANCOUNT 12

CRGB striptop[STRIPCOUNT];
CRGB stripbottom[STRIPCOUNT];
CRGB fanright1[FANCOUNT];
CRGB fanright2[FANCOUNT];
CRGB fanleft[FANCOUNT];
CRGB cooler[FANCOUNT];

static CRGB *fans[4] = {fanleft, cooler, fanright1, fanright2};
static uint8_t fanCount = sizeof(fans) / sizeof(fans[0]);

typedef struct ColorRange
{
  uint8_t startHue;
  uint8_t endHue;
} ColorRange;

ColorRange rg = {0, 96};

ColorRange fg[] = {{96, 192},{192,96},{255,192},{192,255}};

static uint8_t startLed = 0;
static uint8_t currentColor = 0;
static uint8_t currentRing = 0;

void setup()
{
  delay(1000); // 3 second delay for recovery

  //Serial.begin(9600);

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<NEOPIXEL, 2>(fanleft, STRIPCOUNT).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<NEOPIXEL, 3>(cooler, FANCOUNT).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<NEOPIXEL, 4>(fanright1, FANCOUNT).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<NEOPIXEL, 5>(striptop, STRIPCOUNT).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<NEOPIXEL, 6>(fanright2, FANCOUNT).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<NEOPIXEL, 7>(stripbottom, STRIPCOUNT).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}

void rollingGradient(CRGB *led, uint8_t ledcount, uint8_t starthue, uint8_t endhue, bool sparkle = false)
{
  uint8_t range;
  bool isForward;
  if(starthue < endhue)
  {
    range = endhue - starthue;
    isForward = true;
  }
  else
  {
    range = starthue - endhue;
    isForward = false;
  }
  
  uint8_t step = range / ledcount;
  for (uint8_t i = 0; i < ledcount; i++)
  {
    uint8_t color = starthue + (i * step * (isForward ? 1 : -1));
    uint8_t currentLed = ((startLed % ledcount) + i) % ledcount;
    led[currentLed] = CHSV(color, 255, 255 - (i * (255 / ledcount)));
    if (sparkle)
    {
      if (random8() > 254)
      {
        led[currentLed] = CHSV(0, 0, 255);
      }
    }
  }
}

void RingsWithSolidStrips()
{
  EVERY_N_MILLISECONDS(100)
  {

    fill_solid(striptop, STRIPCOUNT, CHSV(rg.startHue, 255, 200));
    fill_solid(stripbottom, STRIPCOUNT, CHSV(rg.endHue, 255, 80));
    rollingGradient(cooler, FANCOUNT, rg.startHue, rg.endHue);
    rollingGradient(fanleft, FANCOUNT, rg.startHue, rg.endHue);
    rollingGradient(fanright1, FANCOUNT, rg.startHue, rg.endHue);
    rollingGradient(fanright2, FANCOUNT, rg.startHue, rg.endHue);
    FastLED.show();
    startLed++;
  }

  EVERY_N_SECONDS(30)
  {
    rg.startHue += 96;
    rg.endHue += 96;
  }
}

void FloatingRingWithSolid()
{
  uint8_t colorSize = sizeof(fg) / sizeof(fg[0]);
  ColorRange rg = fg[currentColor % colorSize];
  uint8_t i = 0;

  EVERY_N_MILLISECONDS(100)
  {

    fill_gradient(striptop, STRIPCOUNT, CHSV(rg.endHue, 255, 200), CHSV(rg.startHue, 255, 200));
    fill_gradient(stripbottom, STRIPCOUNT, CHSV(rg.endHue, 255, 80), CHSV(rg.startHue, 255, 80));

    for (i = 0; i < fanCount; i++)
    {
      if (i < currentRing % fanCount)
      {
        fill_solid(fans[i], FANCOUNT, CHSV(rg.startHue, 255, 200));
      }
      else if (i == currentRing % fanCount)
      {
        rollingGradient(fans[i], FANCOUNT, rg.startHue, rg.endHue);
      }
      else
      {
        fill_solid(fans[i], FANCOUNT, CHSV(rg.endHue, 255, 200));
      }
    }

    FastLED.show();
    startLed++;
  }

  EVERY_N_SECONDS(2)
  {
    currentRing++;
    if (currentRing % fanCount == 0)
    {
      currentColor++;
    }
  }
}

void loop()
{
  FloatingRingWithSolid();
}

#include <FastLED.h>

#define BRIGHTNESS 200
#define COOLERCOUNT 12
#define STRIPCOUNT 12
#define FANCOUNT 8

CRGB strip[STRIPCOUNT];
CRGB fanright[FANCOUNT];
CRGB cooler[COOLERCOUNT];
CRGB fanleft[FANCOUNT];

static uint8_t startled = 0;
static uint8_t starthue = 0;
static uint8_t endhue = 32;

typedef struct HSVGradient
{
  uint8_t starthue;
  uint8_t endhue;
} HSVGradient;

void setup()
{
  delay(1000); // 3 second delay for recovery

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<NEOPIXEL, 2>(strip, STRIPCOUNT).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<NEOPIXEL, 3>(fanright, FANCOUNT).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<NEOPIXEL, 4>(fanleft, FANCOUNT).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<NEOPIXEL, 5>(cooler, COOLERCOUNT).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}

void rollingGradient(CRGB *led, uint8_t ledcount, uint8_t starthue, uint8_t endhue)
{
  uint8_t step = (endhue - (starthue)) / ledcount;
  for (uint8_t i = 0; i < ledcount; i++)
  {
    uint8_t color = (starthue) + (i * step);

    led[(startled + i) % ledcount] = CHSV(color, 255, 255);
  }
}

void rollingRing(CRGB *led, uint8_t ledcount, uint8_t hue, uint8_t saturation, uint8_t brighteness = 255)
{
  uint8_t i;

  for (i = 0; i < ledcount; i++)
  {
    led[i].setHSV(0, 0, 0);
  }

  uint8_t lightleds = ledcount;
  uint8_t offset = brighteness / lightleds;

  for (i = 0; i < lightleds; i++)
  {
    led[(startled + i) % ledcount].setHSV(hue - i * 4, saturation, brighteness - (offset * i));
  }
}

void LightedLowBrightness(uint8_t hue)
{
  fill_solid(strip, STRIPCOUNT, CRGB(255, 255, 255));
  rollingRing(cooler, COOLERCOUNT, hue, 255, 255);
  rollingRing(fanleft, FANCOUNT, hue, 255, 127);
  rollingRing(fanright, FANCOUNT, hue, 255, 127);
}

void SideToSideWithRing(uint8_t starthue, uint8_t endhue)
{
  rollingRing(cooler, COOLERCOUNT, starthue + 16, 255);
  fill_gradient(strip, STRIPCOUNT, CHSV(endhue, 255, 255), CHSV(starthue, 255, 255));
  fill_solid(fanleft, FANCOUNT, CHSV(starthue, 255, 255));
  fill_solid(fanright, FANCOUNT, CHSV(endhue, 255, 255));
}

void SideToSideGradient(uint8_t starthue, uint8_t endhue) 
{
  rollingGradient(cooler, COOLERCOUNT, starthue, endhue);
  fill_gradient(strip, STRIPCOUNT, CHSV(endhue, 255, 255), CHSV(starthue, 255, 255));
  fill_solid(fanleft, FANCOUNT, CHSV(starthue, 255, 255));
  fill_solid(fanright, FANCOUNT, CHSV(endhue, 255, 255));
}

void FillSolidWithRingWhite()
{
  rollingRing(cooler, COOLERCOUNT, 0, 0);
  fill_solid(strip, STRIPCOUNT, CRGB(255, 255, 255));
  fill_solid(fanleft, FANCOUNT, CRGB(255, 255, 255));
  fill_solid(fanright, FANCOUNT, CRGB(255, 255, 255));
}

void AllRollingRing(uint8_t starthue, uint8_t endhue)
{
  rollingGradient(cooler, COOLERCOUNT, starthue, endhue);
  fill_gradient(strip, STRIPCOUNT, CHSV(endhue, 255, 255), CHSV(starthue, 255, 255));
  rollingGradient(fanleft, FANCOUNT, starthue, endhue);
  rollingGradient(fanright, FANCOUNT, starthue, endhue);
}

void loop()
{

  EVERY_N_MILLISECONDS(120)
  {
    FastLED.show();
    LightedLowBrightness(starthue);
    startled++;
  }

  EVERY_N_SECONDS(30)
  {
    starthue += 32;
    endhue += 32;
  }
}

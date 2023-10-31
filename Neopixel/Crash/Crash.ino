#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#define PIN        6 // On Trinket or Gemma, suggest changing this to 1
#define NUMPIXELS 205 
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#define DELAYVAL 5 // Time (in milliseconds) to pause between pixels

int rev = (NUMPIXELS);
void setup() {
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)

}

void loop()
{
  pixels.clear();
  for(int i=0; i<(NUMPIXELS); i++)
  {
    pixels.setPixelColor(i, pixels.Color(0, 0, 255));
    pixels.show();
    
    pixels.setPixelColor(rev, pixels.Color(255, 0, 0));
    rev--;
    pixels.show();
    delay(DELAYVAL);
  }
  rev = (NUMPIXELS);
}
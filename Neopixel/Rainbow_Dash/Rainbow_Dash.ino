#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#define PIN        6 // On Trinket or Gemma, suggest changing this to 1

#define NUMPIXELS 24 

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 5 // Time (in milliseconds) to pause between pixels

int j = 0;

void setup() {
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.
  Serial.begin(9600);
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)

  
}

void loop()
{
  //pixels.clear();

  for(int i=0; i<NUMPIXELS; i++)
  {
    switch(j)
    {
      case 0:
        pixels.setPixelColor(i, pixels.Color(255, 0, 0));
        j++;
        break;
      case 1:
        pixels.setPixelColor(i, pixels.Color(255, 100, 0));
        j++;
        break;
      case 2:
        pixels.setPixelColor(i, pixels.Color(255, 255, 0));
        j++;
        break;
      case 3:
        pixels.setPixelColor(i, pixels.Color(0, 255, 0));
        j++;
        break;
      case 4:
        pixels.setPixelColor(i, pixels.Color(0, 255, 255));
        j++;
        break;
      case 5:
        pixels.setPixelColor(i, pixels.Color(0, 0, 255));
        j++;
        break;
      case 6:
        pixels.setPixelColor(i, pixels.Color(255, 0, 255));
        j++;
        break;
    }
    pixels.show();
    Serial.print(j);
    if(j >= 7)
    {
      j = 0;
    }
    delay(DELAYVAL);
  }
}

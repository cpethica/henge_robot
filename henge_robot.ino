#include "Adafruit_WS2801.h"
#include "SPI.h" // Comment out this line if using Trinket or Gemma
#include <FastLED.h>

#include "arrays.cpp"

// set up ws2812b strip
#define DATA_PIN_1 5
#define NUM_LEDS_1 12
CRGB leds_1[NUM_LEDS_1];
int power = 0;   // power bar current position

// set up ws2812b matrix
#define DATA_PIN_3 9
#define NUM_LEDS_3 64
CRGB leds_3[NUM_LEDS_1];
int pic = 0;   // matrix picture current position
int num_pics = 4;   // number of pictures available

// set up ws2801 strip
#define DATA_PIN_2 2
#define CLOCK_PIN_2 3
#define NUM_LEDS_2 19
CRGB leds_2[NUM_LEDS_2];

#include "Adafruit_NeoTrellis.h"
Adafruit_NeoTrellis trellis;
boolean button_state[16] = {false};        // state for each button
boolean button_state_previous[16] = {false};        // state for each button

//define a callback for key presses
TrellisCallback blink(keyEvent evt){
  // Check is the pad pressed?
  if (evt.bit.EDGE == SEESAW_KEYPAD_EDGE_RISING) {
    button_state_previous[evt.bit.NUM] = button_state[evt.bit.NUM];  // store state before it is changed
    button_state[evt.bit.NUM] = !button_state[evt.bit.NUM];   // change button state
    delay(50);   // debounce
    // button for power bar
    if (evt.bit.NUM == 15) {      // increment counter if button 15 is pressed
      if (power < NUM_LEDS_1) {  
        power += 1;
      }
      else {                      // reset at end of led strip
        power = 0;
      }
    }
    // button for matrix
    if (evt.bit.NUM == 14) {      // increment counter if button 14 is pressed
      if (pic < num_pics) {  
        pic += 1;
      }
      else {                      // loop through available matrix pics
        pic = 0;
      }
    }        
  }
  return 0;
}

void setup() {
  
  Serial.begin(9600);
  trellis.begin();

  FastLED.addLeds<WS2812B, DATA_PIN_1, GRB>(leds_1, NUM_LEDS_1);  // GRB ordering is typical
  FastLED.addLeds<WS2801, DATA_PIN_2, CLOCK_PIN_2, GBR>(leds_2, NUM_LEDS_2);
  FastLED.addLeds<WS2812B, DATA_PIN_3, GRB>(leds_3, NUM_LEDS_3);  // GRB ordering is typical
  
  for (int i = 0; i<12; i++) {
    leds_1[i] = CRGB::Black;
  }
  FastLED.setBrightness(CRGB(40,40,40));
  FastLED.show();
  //
  // set up neotrellis button array
  //
  //activate all keys and set callbacks
  for(int i=0; i<NEO_TRELLIS_NUM_KEYS; i++){
    trellis.activateKey(i, SEESAW_KEYPAD_EDGE_RISING);
    trellis.activateKey(i, SEESAW_KEYPAD_EDGE_FALLING);
    trellis.registerCallback(i, blink);
  }
   //do a little animation to show we're on
  for (uint16_t i=0; i<trellis.pixels.numPixels(); i++) {
    trellis.pixels.setPixelColor(i, Wheel(map(i, 0, trellis.pixels.numPixels(), 0, 255)));
    trellis.pixels.show();
    delay(50);
  }
  for (uint16_t i=0; i<trellis.pixels.numPixels(); i++) {
    trellis.pixels.setPixelColor(i, 0x000000);
    trellis.pixels.show();
    delay(50);
  }
}


void loop() {

  trellis.read();  // interrupt management does all the work! :)
  delay(20); //the trellis has a resolution of around 60hz
 
  // check button status and update pixels
  for (int i = 0; i<trellis.pixels.numPixels(); i++) {
    if (button_state[i] == true) {
      trellis.pixels.setPixelColor(i, Wheel(map(i, 0, trellis.pixels.numPixels(), 0, 255))); //on rising
      
      leds_2[i] = Wheel(map(i, 0, trellis.pixels.numPixels(), 0, 255));
      //Serial.println(i);
    }
    else if (button_state[i] == false) {
      trellis.pixels.setPixelColor(i, 0); //off falling
      leds_2[i] = CRGB::Black;
    }
  }

  // increment led strip for power up feature
  for (int i = 0; i<power; i++) {   
      leds_1[i] = CRGB::White;      // light power bar positions
  }
  
  for (int i = power; i<NUM_LEDS_1; i++) {    // switch off non lit positions
        leds_1[i] = CRGB::Black;
  }

  // led matrix
  for(int i = 0; i < NUM_LEDS_3; i++)
  {
    leds_3[i] = ledarray1[i];
  }
  // Turn on/off the neopixels!
  trellis.pixels.show();
  FastLED.show();      // write all the pixels out
}

/* Helper functions */

// Create a 24 bit color value from R,G,B
uint32_t Color(byte r, byte g, byte b)
{
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}

//Input a value 0 to 255 to get a color value.
//The colours are a transition r - g -b - back to r

uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return trellis.pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return trellis.pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return trellis.pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  return 0;
}

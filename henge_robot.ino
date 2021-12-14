#include "Adafruit_WS2801.h"
#include "SPI.h" // Comment out this line if using Trinket or Gemma

#include "Adafruit_NeoTrellis.h"

Adafruit_NeoTrellis trellis;

boolean button_state[16] = {false};        // state for each button

uint8_t dataPin  = 2;    // Yellow wire on Adafruit Pixels
uint8_t clockPin = 3;    // Green wire on Adafruit Pixels

Adafruit_WS2801 strip = Adafruit_WS2801(19, dataPin, clockPin);

//define a callback for key presses
TrellisCallback blink(keyEvent evt){
  // Check is the pad pressed?
  if (evt.bit.EDGE == SEESAW_KEYPAD_EDGE_RISING) {
    button_state[evt.bit.NUM] = !button_state[evt.bit.NUM];   // change button state
  }
  return 0;
}

void setup() {
  
  Serial.begin(9600);
  trellis.begin();
   
  //
  // set up led strip
  //
  strip.begin();
  // Update LED contents, to start they are all 'off'
  strip.show();
  
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
      strip.setPixelColor(i, Color(255, 0, 0));
    }
    else if (button_state[i] == false) {
      trellis.pixels.setPixelColor(i, 0); //off falling
      strip.setPixelColor(i, 0);
    }
  }
  // Turn on/off the neopixels!
  trellis.pixels.show();
  strip.show();   // write all the pixels out

  

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


void turn_off(){
  for (int h=0;h  < NUM_LEDS; h++) {
      // Turn our current led on to white, then show the leds
      leds[h] = CRGB::Black;
    
  }
  // Show the leds (only one of which is set to white, from above)
  FastLED.show();
}


void transition(CRGB color){
  for (int h=0;h  < NUM_LEDS; h++) {
      // Turn our current led on to white, then show the leds
      leds[h] = color;
    
  }
  // Show the leds (only one of which is set to white, from above)
  FastLED.show();
}

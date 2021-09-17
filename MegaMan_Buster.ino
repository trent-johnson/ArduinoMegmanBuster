#include "FastLED.h"

#define NUM_LEDS 93
#define DATA_PIN 3
#define CLOCK_PIN 13
#define BUTTON_PIN 9

//The boundaries of the various LED rings
int ringArray[] = {0,32,56,72,84,93};

int buttonState = 0;

//Initial color selection
int r = 60;
int g = 188;
int b = 252;

//Looping variables
unsigned long prevMillis = 0;
unsigned long currentMillis = 0;
unsigned long animationMillis = 0;

//Animation helpers
int rowCount = 0;
int loopInterval = 100;
int animationInterval = 5000;

//Animation to run
// 0 = none
// 1 = charging
int animate = 0;

// Define the array of leds
CRGB leds[NUM_LEDS];

void setup() { 
  Serial.begin(115200);
  Serial.println("Setup Started");
  pinMode(BUTTON_PIN, INPUT);
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.clear();
  FastLED.show();
  Serial.println("Setup Complete");
}

void loop() { 
  
  Serial.println("Loop Begin");
  buttonState = digitalRead(BUTTON_PIN);
  currentMillis = millis();
  if (buttonState == HIGH && animate == 0) {
    Serial.println("Clicked!");

    animate = 1;
    animationMillis = currentMillis;
    
  }

  if(currentMillis - animationMillis > animationInterval) {
    animate = 0;
    loopInterval = 100;
    FastLED.clear();
    FastLED.show();
    setLedBlue();
  }

  //Charge Animation
  if(animate == 1 && currentMillis - prevMillis > loopInterval) {    
    LightRing(rowCount);
  }
  

}

void LightRing(int row) {
    FastLED.clear();
    for(int led = ringArray[row]; led < ringArray[row+1]; led++) { 
        leds[led].setRGB( r, g, b); 
    }
    FastLED.show();
    prevMillis = currentMillis;
    if(rowCount < 4) {
      rowCount++;
    } else {
      rowCount = 0;
      if(loopInterval < 60) {
        setLedYellow();
      }
      
      if(loopInterval > 10) {
        loopInterval = loopInterval - 5;
      }
    }
}

void setLedBlue() {
    r = 60;
    g = 188;
    b = 252;
}
void setLedYellow() {
    r = 251;
    g = 219;
    b = 60;
}

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

//Pulse Color
//0 = yellow
//1 = red
int pulseColor = 0;

int pulseCount = 0;
int pulseBright = 100;
bool pulseInc = false;

int warmBright = 2;
int fireBright = 200;

int loopInterval = 100;
int animationInterval = 5000;


//Animation to run
// 0 = none
// 1 = charging
// 2 = idle pulse
// 3 = warm up
int animate = 3;

// Define the array of leds
CRGB leds[NUM_LEDS];

void PulseRing(int bright, int startRow = -1, int endRow = -1, int pulseColor = 0);

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
  
  buttonState = digitalRead(BUTTON_PIN);
  currentMillis = millis();
  if (buttonState == HIGH && animate != 1) {
    Serial.println("Clicked!");

    animate = 1;
    animationMillis = currentMillis;
    
    setLedBlue();
    
  }

  //Reset after charge animation (1) or cooldown (5) animation
  if(currentMillis - animationMillis > animationInterval && animate == 1) {
    animate = 4; //Send to fire!
    loopInterval = 100;
    FastLED.clear();
    FastLED.show();
  } else if (currentMillis - animationMillis > animationInterval && animate == 5) {
    animate = 3; //Send to warm up
    FastLED.clear();
    FastLED.show();
  }

  //Charge Animation
  if(animate == 1 && currentMillis - prevMillis > loopInterval) {    
    LightRing(rowCount);
  }

  //Idle Pulse Animation
  if(animate == 2 && currentMillis - prevMillis > loopInterval/2) {
    PulseRing(pulseBright);

    //Should we increase or not
    if(pulseBright < 20 && pulseInc == false) {
      pulseInc = true;
    } else if (pulseBright >= 100 && pulseInc == true) {
      pulseInc = false;
    }
    //Chamge the brightness
    if(pulseInc == true) {
      pulseBright = pulseBright + 2;
    } else {
      pulseBright = pulseBright - 2;
    } 
  }

  //Warm up animation
  if(animate == 3 && currentMillis - prevMillis > loopInterval/2) {
    warmBright++;
    if(warmBright < 15) {
      PulseRing(warmBright, 4, 5);
    } else if (warmBright < 40) {
      PulseRing(warmBright, 3, 5); 
    } else if (warmBright < 60) {
      PulseRing(warmBright, 2, 5);
    } else if (warmBright < 80) {
      PulseRing(warmBright, 1, 5);
    } else {
      PulseRing(warmBright);
    }
    
    if(warmBright == 100) {
      warmBright = 0;
      pulseBright = 100;
      animate = 2;
    }
  }

  //Fire animation
  if(animate == 4 && currentMillis - prevMillis > loopInterval/20) {
    fireBright = fireBright - 3;
    if(fireBright < 50) {
      PulseRing(fireBright, 4, 5, 1);
    } else if (fireBright < 90) {
      PulseRing(fireBright, 3, 5, 1); 
    } else if (fireBright < 120) {
      PulseRing(fireBright, 2, 5, 1);
    } else if (fireBright < 150) {
      PulseRing(fireBright, 1, 5, 1);
    } else {
      PulseRing(fireBright, 0, 5, 1);
    }

    if(fireBright <= 10) {
      animationMillis = currentMillis - 3500;
      animate = 5; //Set to cooldown
      fireBright = 200;
      warmBright = 0;
    }
  }

  //Cooldown
  if(animate == 5 ) {
    PulseRing(10, 4, 5, 1);
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

void PulseRing(int bright, int startRow = -1, int endRow = -1, int pulseColor = 0) {
  Serial.print("Pulsing to brightness ");
  Serial.println(bright);
  
  prevMillis = currentMillis;
  FastLED.clear();
  //Affect the entire array
  if(startRow == -1) {
    for(int led = ringArray[0]; led < ringArray[5]; led++) {
      if(pulseColor == 0) {
        leds[led].setRGB(bright, bright / 1.5, 0);
      } else if(pulseColor == 1) {
        leds[led].setRGB(bright, 0, 0);
      }
    }
  } else {    
    for(int led = ringArray[startRow]; led < ringArray[endRow]; led++) { 
      if(pulseColor == 0) {
        leds[led].setRGB(bright, bright / 1.5, 0);
      } else if(pulseColor == 1) {
        leds[led].setRGB(bright, 0, 0);
      }
    }
  }
  FastLED.show();
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

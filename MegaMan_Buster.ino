#include "FastLED.h"
#include "DFPlayerMini_Fast.h"

#if !defined(UBRR1H)
  #include "SoftwareSerial.h"
  SoftwareSerial mySerial(10, 11); // RX, TX
#endif


DFPlayerMini_Fast myMP3;

#define NUM_LEDS 93
#define DATA_PIN 3
#define CLOCK_PIN 13
#define BUTTON_PIN 9
#define MUSIC_PIN 7

//The boundaries of the various LED rings
int ringArray[] = {0,32,56,72,84,93};

//Settings
int volume = 29;
int brightness = 100;
int menu = 0; // 0 = volume, 1 = brightness

//Buster Modes 
// 1 = normal
// 2 = settings
int busterMode = 1;

//Button States
int musicState = 0;
int buttonState = 0;
int btnPress = 0;
int musicPress = 0;

//Initial color selection
int r = 60;
int g = 188;
int b = 252;

//Looping variables
unsigned long prevMillis = 0;
unsigned long currentMillis = 0;
unsigned long animationMillis = 0;
unsigned long chargeMillis = 0;
unsigned long musicMillis = 0;

int loopInterval = 100;
int animationInterval = 5000;

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


//Animation to run
// 0 = none
// 1 = charging
// 2 = idle pulse
// 3 = warm up
// 4 = fire
// 5 = cooldown
int animate = 3;

//Music
// 1 = Charging
// 2 = Zero Theme
// 3 = Full Charge shot
// 4 = Half Charge shot
// 5 = Armadelo Stage
// 6 = Boomer Stage
// 7 = Bridge Stage
// 8 = Chill Stage
// 9 = Flame Stage
// 10 = Spark Stage
// 11 = Sting Stage
// 12 = Storm Stage
// 13 = Octo Stage 
// 14 = Final Stage
// 15 = Intro
// 16 = Basic Buster

int musicArray[] = {2,5,6,7,8,9,10,11,12,13,14};

// Define the array of leds
CRGB leds[NUM_LEDS];

void PulseRing(int bright, int startRow = -1, int endRow = -1, int pulseColor = 0);

void setup() { 
  Serial.begin(115200);
  Serial.println("Setup Started");
  pinMode(BUTTON_PIN, INPUT);
  pinMode(MUSIC_PIN, INPUT);
  
  Serial.println("Buttons Added");
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.clear();
  
  Serial.println("LEDs added");

  mySerial.begin(9600);


  
  if(!myMP3.begin(mySerial, true)) {
    Serial.println("DFMini Not Intialized.");
    while(true);
  }
  
  Serial.println("DFplayer Added");
  myMP3.volume(volume);
  
  Serial.println("Set volume");
  delay(2000);
  myMP3.play(15);
  
  Serial.println("Playing intro");


  FastLED.show();
  Serial.println("Setup Complete");
}

void loop() { 
  
  buttonState = digitalRead(BUTTON_PIN);
  musicState = digitalRead(MUSIC_PIN);
  currentMillis = millis();

  //Button actions in settings mode
  if(busterMode == 2) {
    
    //Decrease button (music)
    if (musicState == HIGH)
    {
      if(musicPress == 0) {
        musicMillis = currentMillis;
      }
      musicPress = 1;
    } else if (musicState == LOW && musicPress > 0 && currentMillis - musicMillis < 2000) {
      if(menu == 0) {
        //When released decrease volume     
        myMP3.decVolume();                
        Serial.println("Decrease volume");
      } else if (menu == 1) {
        //When released decrease volume      
        if(brightness <= 0) {
          brightness = 0;
        } else {
          brightness = brightness - 10;
        }
        Serial.println("Decrease brightness to");
        Serial.println(brightness); 
        animate = 2;
        FastLED.show();
      }
      myMP3.play(3); //Half Charge confirmation
      musicPress = 0;
    } else  if (musicState == LOW && musicPress > 0) {
      //Button was held longer then 2 seconds - Enter normal mode
      Serial.println("Entering normal mode");
      busterMode = 1;
      musicPress = 0;
      FastLED.clear();
      FastLED.show();
      myMP3.play(16); //Confirmation noise - basic buster
      animate = 3;
    }

    //Increase button (Charge)
    if (buttonState == HIGH) {
      //If you've just clicked the button (first frame)
      if(btnPress == 0) {   
        chargeMillis = currentMillis;
      }
      btnPress = 1;
    } 
    else if(buttonState == LOW && btnPress > 0 && currentMillis - chargeMillis < 2000) { //Released Button
      if(menu == 0) {
        //When released decrease volume      
        
        myMP3.incVolume();
        Serial.println("Increase volume");
      } else if (menu == 1) {
        //When released decrease volume      
        if(brightness >= 100) {
          brightness = 100;
        } else {
          brightness = brightness + 10;
        }
        Serial.println("Increase brightness to");
        Serial.println(brightness); 
        animate = 2;
        FastLED.show();
      }
      btnPress = 0;
      myMP3.play(3);  //Half Charge confirmation
    } else if(buttonState == LOW && btnPress > 0 ) {
      //change menu
      if(menu == 0) {
        menu = 1;
      } else {
        menu = 0;
      }
      
      btnPress = 0;
      myMP3.play(4);  //Fully Chage shot confirmation
    }
  }


  //Button actions if in normal mode
  if(busterMode == 1) {
    
    //Pressing Music Button
    if (musicState == HIGH)
    {
      if(musicPress == 0) {
        musicMillis = currentMillis;
      }
      musicPress = 1;
    } else if (musicState == LOW && musicPress > 0 && currentMillis - musicMillis < 2000) {
      //When released      
      myMP3.play(musicArray[random(0,10)]);
      musicPress = 0;
    } else if (musicState == LOW && musicPress > 0) {
      //Button was held longer then 2 seconds - Enter settings mode
      Serial.println("Entering settings mode");
      busterMode = 2;
      musicPress = 0;
      FastLED.clear();
      FastLED.show();
      myMP3.play(16); //Confirmation noise - basic buster
    }
    
    //Current Pressing Fire button
    if (buttonState == HIGH) {
      //If you've just clicked the button (first frame)
      if(btnPress == 0) {   
        chargeMillis = currentMillis;
        setLedBlue(); 
        Serial.println("Play Charging");
        myMP3.play(1);  
        animate = 1;
      }
      btnPress = 1;
    } 
    else if(buttonState == LOW && btnPress > 0) { //Released Button
  
      if(currentMillis - chargeMillis > 2500) {
        Serial.println("Play Fully Charged shot");
        myMP3.play(3); 
      } else {
        Serial.println("Play Partial Charged shot");
        myMP3.play(4);
      }
      btnPress = 0;
      fireBright = 200;
      animate = 4; //Send to fire!
      
      loopInterval = 100;
      ClearRing();
      FastLED.show();
    }
  }
  
  //Reset after charge animation (1) or cooldown (5) animation
  if (currentMillis - animationMillis > animationInterval && animate == 5) {
    animate = 3; //Send to warm up
    ClearRing();
    FastLED.show();
  }

  //Charge Animation
  if(animate == 1 && currentMillis - prevMillis > loopInterval/2) {    
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
    ClearRing();
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
   
  prevMillis = currentMillis;
  ClearRing();
  //Affect the entire array
  if(startRow == -1) {
    for(int led = ringArray[0]; led < ringArray[5]; led++) {
      if(pulseColor == 0) {
        leds[led].setRGB((int) bright * (brightness * 0.01), (int) (bright / 4) * (brightness * 0.01), 0);
      } else if(pulseColor == 1) {
        leds[led].setRGB((int) bright * (brightness * 0.01), 0, 0);
      }
    }
  } else {    
    for(int led = ringArray[startRow]; led < ringArray[endRow]; led++) { 
      if(pulseColor == 0) {
        leds[led].setRGB((int) bright * (brightness * 0.01), (int) (bright / 4) * (brightness * 0.01), 0);
      } else if(pulseColor == 1) {
        leds[led].setRGB((int) bright * (brightness * 0.01), 0, 0);
        
      }
    }
  }
  FastLED.show();
}

void ClearRing() {
  for(int led = ringArray[0]; led < ringArray[5]; led++) {
    leds[led].setRGB(0,0,0);
  }
}

void setLedBlue() {
    r = (int) 60 * (brightness * 0.01);
    g = (int) 188 * (brightness * 0.01);
    b = (int) 252 * (brightness * 0.01);
}
void setLedYellow() {
    r = (int) 251 * (brightness * 0.01);
    g = (int) 219 * (brightness * 0.01);
    b = (int) 60 * (brightness * 0.01);
}

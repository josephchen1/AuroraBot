#include <LiquidCrystal_I2C.h>
#include <TimeLib.h>
#include <Vector.h>
#include <DHT.h>
#include <Adafruit_NeoPixel.h>
#define DHTPIN A0
#define DHTTYPE DHT11

float pf;
float pc;
static int pinA = 2;
static int pinB = 3;
static int CLK = 4;
static int LED = 11;
static int MENUBTN = 6;

byte degree[8] = {
  B00110,
  B01001,
  B01001,
  B00110,
  B00000,
  B00000,
  B00000,
  B00000,
};

volatile byte aFlag = 0;
volatile byte bFlag = 0;
volatile byte encoderPos = 0;
volatile byte oldEncPos = 0;
volatile byte reading = 0;

int i = 1;
int counter = 0;
int currentStateCLK;
int lastStateCLK;
String currentDir = "";
unsigned long lastButtonPress = 0;
float TimeNow2;
float TimeNow1;
boolean ButtonPressed = false;

int timeNum = 1;
int month1;
int day1;
int year1;
int hour1;
int min1;

int daysActive = 0;
int avgSleepTime = 0;
int totalSleepTime = 0;
int goalSleepTime = 8;
int avgMidday = 0;
int totalMidday = 0;
int goalMidday = 8;

LiquidCrystal_I2C lcd(0x27, 20, 4);
DHT dht(DHTPIN, DHTTYPE);
Adafruit_NeoPixel strip = Adafruit_NeoPixel
                          (28, LED, NEO_GRB + NEO_KHZ800);

Vector<int> sleepData;
Vector<int> middayData;
void setup() {
  //Don't change anything below
  pinMode(pinA, INPUT_PULLUP);
  pinMode(pinB, INPUT_PULLUP);
  pinMode(CLK, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), PinA, FALLING);
  attachInterrupt(digitalPinToInterrupt(3), PinB, FALLING);
  attachInterrupt(digitalPinToInterrupt(4), button, FALLING);
  Serial.begin(9600);
  lastStateCLK = digitalRead(CLK);
  TimeNow1 = millis();
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
  strip.begin();
  strip.setBrightness(85);
  strip.show();
}
void button() {
  int btnState = digitalRead(CLK);

  //If we detect LOW signal, button is pressed
  if (btnState == LOW) {
    if (millis() - lastButtonPress > 50) {
      ButtonPressed = true;
      Serial.print("Button Pressed");
    }
    lastButtonPress = millis();
  } else {
    ButtonPressed = false;
  }
  delay(1);

}
void PinA() {
  reading = PIND & 0xC;
  if (reading == B00001100 && aFlag) {
    encoderPos --;
    bFlag = 0;
    aFlag = 0;
  }
  else if (reading == B00000100) {
    bFlag = 1;
  }
}
void PinB() {
  reading = PIND & 0xC;
  if (reading == B00001100 && bFlag) {
    encoderPos ++;
    bFlag = 0;
    aFlag = 0;
  }
  else if (reading == B00001000) {
    aFlag = 1;
  }
}
void morningCheckIn() {
  button();
  lcd.print("How many hours");
  lcd.setCursor(0, 1);
  lcd.print("did you sleep");
  lcd.setCursor(0, 2);
  lcd.print("last night?");
  lcd.setCursor(0, 3);
  while (!ButtonPressed) {
    button();
    lcd.setCursor(18, 3);
    if (encoderPos == 25) {
      encoderPos = 24;
    } else if (encoderPos == 255) {
      encoderPos = 0;
    }
    lcd.print(encoderPos);
  }
  if (ButtonPressed) {
    int sleepTime = encoderPos;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("You slept for");
    lcd.setCursor(0, 1);
    lcd.print(sleepTime);
    lcd.setCursor(0, 2);
    lcd.print("hours");
    delay(3000);
    sleepData.PushBack(sleepTime);
    totalSleepTime += sleepTime;
    avgSleepTime = totalSleepTime / daysActive;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("You average sleep");
    lcd.setCursor(0, 1);
    lcd.print("time is ");
    lcd.setCursor(8, 1);
    lcd.print(avgSleepTime);
    lcd.setCursor(10, 1);
    lcd.print("hours");
    delay(3000);
    lcd.clear();
    if (avgSleepTime < goalSleepTime) {
      lcd.setCursor(0, 0);
      lcd.print("Oh no! Why did you");
      lcd.setCursor(0, 1);
      lcd.print("sleep less today?");
      lcd.setCursor(0, 2);
      lcd.print("1. Stress");
      lcd.setCursor(0, 3);
      lcd.print("2. Caffeine");
      lcd.setCursor(18, 3);
      button();
      while (!ButtonPressed) {
        button();
        lcd.setCursor(18, 3);
        if (encoderPos >= 3) {
          encoderPos = 2;
        } else if (encoderPos == 0) {
          encoderPos = 1;
        }
        lcd.print(encoderPos);
      }
      if (ButtonPressed) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("You said that");
        lcd.setCursor(0, 1);
        if (encoderPos == 1) {
          lcd.print("stress is why you");
          lcd.setCursor(0, 2);
          lcd.print("didn't sleep so well");
          delay(3000);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Can we do some");
          lcd.setCursor(0, 1);
          lcd.print("breathing exercises");
          lcd.setCursor(0, 2);
          lcd.print("to help with that?");
          button();
          while (!ButtonPressed) {
            button();
            lcd.setCursor(10, 3);
            if (encoderPos == 3) {
              encoderPos = 2;
            } else if (encoderPos == 255) {
              encoderPos = 1;
            }
            if (encoderPos == 1) {
              lcd.print("Yes");
            } else {
              lcd.print("Nah");
            }
            if (ButtonPressed) {
              if (encoderPos == 1) {
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Great!");
                lcd.setCursor(0, 1);
                lcd.print("Let's destress!");
                lcd.setCursor(0, 2);
                lcd.print("Entering breathing");
                lcd.setCursor(0, 3);
                lcd.print("exercise...");
                delay(3000);
                lcd.clear();
                breathingExercise();
              }
            }
          }
        }
        if (encoderPos == 2) {
          lcd.print("caffeine is why you");
          lcd.setCursor(0, 2);
          lcd.print("didn't sleep so well");
          delay(3000);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Can we try and drink");
          lcd.setCursor(0, 1);
          lcd.print("something that's not");
          lcd.setCursor(0, 2);
          lcd.print("coffee to help?");
          button();
          while (!ButtonPressed) {
            button();
            lcd.setCursor(10, 3);
            if (encoderPos == 3) {
              encoderPos = 2;
            } else if (encoderPos == 255) {
              encoderPos = 1;
            }
            if (encoderPos == 1) {
              lcd.print("Yes");
            } else {
              lcd.print("Nah");
            }
            if (ButtonPressed) {
              if (encoderPos == 1) {
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Great!");
                lcd.setCursor(0, 1);
                lcd.print("Let's detox!");
                lcd.setCursor(0, 2);
                lcd.print("Entering detox");
                lcd.setCursor(0, 3);
                lcd.print("drinks menu page...");
                delay(3000);
                lcd.clear();
                detoxDrink();
              }
            }
          }
        }
      }
    }
  }
}
void middayCheckIn() {
  button();
  lcd.print("How was your");
  lcd.setCursor(0, 1);
  lcd.print("morning on a scale");
  lcd.setCursor(0, 2);
  lcd.print("of 1 to 10?");
  while (!ButtonPressed) {
    button();
    lcd.setCursor(10, 3);
    if (encoderPos >= 11) {
      encoderPos = 10;
    } else if (encoderPos == 255) {
      encoderPos = 0;
    }
    lcd.print(encoderPos);
  }
  if (ButtonPressed) {
    int middayRating = encoderPos;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("You gave your morning");
    lcd.setCursor(0, 1);
    lcd.print(encoderPos);
    lcd.setCursor(3, 1);
    lcd.print("out of 10");
    delay(3000);
    middayData.PushBack(middayRating);
    totalMidday += middayRating;
    avgMidday = totalMidday / daysActive;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("You average midday");
    lcd.setCursor(0, 1);
    lcd.print("rating is ");
    lcd.setCursor(8, 1);
    lcd.print(avgMidday);
    lcd.setCursor(10, 1);
    lcd.print("hours");
    delay(3000);
    lcd.clear();
    if (avgMidday < goalMidday) {
      lcd.setCursor(0, 0);
      lcd.print(":( Sorry to hear that");
      lcd.setCursor(0, 1);
      lcd.print("Why was it not good?");
      delay(3000);
      //TODO
    }
  }
}
void breathingExercise() {
  int TOTAL_LEDS = 28;
  float MaximumBrightness = 255;
  float SpeedFactor = 0.005;
  float StepDelay = 5;

  // Make the lights breathe
  for (int i = 0; i < 65535; i++) {
    float intensity = MaximumBrightness / 2.0 * (1.0 + sin(SpeedFactor * i));
    strip.setBrightness(intensity);
    for (int ledNumber = 0; ledNumber < TOTAL_LEDS; ledNumber++) {
      strip.setPixelColor(ledNumber, 0, 0, 255);
    }

    strip.show();
    delay(StepDelay);
  }
}
void detoxDrink() {
  button();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("1. Classic Green");
  lcd.setCursor(0, 1);
  lcd.print("2. Choccy Peanut");
  lcd.setCursor(0, 2);
  lcd.print("3. Pina Colada");
  lcd.setCursor(0, 3);
  lcd.print("4. Very Berries");
  while (!ButtonPressed) {
    button();
    lcd.setCursor(18, 3);
    if (encoderPos >= 5) {
      encoderPos = 4;
    } else if (encoderPos == 255) {
      encoderPos = 1;
    }
    lcd.print(encoderPos);
  }
  if (ButtonPressed) {
    int drinkSelection = encoderPos;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("For your drink");
    lcd.setCursor(0, 1);
    lcd.print("simply add all");
    lcd.setCursor(0, 2);
    lcd.print("ingredients in a");
    lcd.setCursor(0, 3);
    lcd.print("and blend them, yum!");
    delay(5000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("You will need");
    lcd.setCursor(0, 1);
    lcd.print("these ingredients!");
    delay(3000);
    lcd.clear();
    if (drinkSelection == 1) {
      lcd.setCursor(0, 0);
      lcd.print("1. a frozen banana");
      lcd.setCursor(0, 1);
      lcd.print("Bananas help you");
      lcd.setCursor(0, 2);
      lcd.print("with digestion!");
      while (!ButtonPressed) {
        button();
      }
      if (ButtonPressed) {
        delay(3000);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("2. a cup of spinach");
        lcd.setCursor(0, 1);
        lcd.print("Spinach benefits");
        lcd.setCursor(0, 2);
        lcd.print("your eye health!");
        while (!ButtonPressed) {
          button();
        }
        if (ButtonPressed) {
          delay(3000);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("3. a cup of milk");
          lcd.setCursor(0, 1);
          lcd.print("Any milk works!");
          lcd.setCursor(0, 2);
          lcd.print("Soy, animal, etc.");
          while (!ButtonPressed) {
            button();
          }
          if (ButtonPressed) {
            delay(3000);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("4. Honey to taste");
            lcd.setCursor(0, 1);
            lcd.print("Everyone needs a");
            lcd.setCursor(0, 2);
            lcd.print("little sweetness");
            lcd.setCursor(0, 3);
            lcd.print("in their life!");
          }
        }
      }
    } else if (drinkSelection == 2) {
      lcd.print("1. a frozen banana");
      lcd.setCursor(0, 2);
      lcd.print("Bananas help you");
      lcd.setCursor(0, 3);
      lcd.print("with digestion!");
      while (!ButtonPressed) {
        button();
      }
      if (ButtonPressed) {
        delay(3000);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("2. a tbsp of peanut");
        lcd.setCursor(0, 1);
        lcd.print("butter.");
        lcd.setCursor(0, 2);
        lcd.print("Peanuts provide you");
        lcd.setCursor(0, 3);
        lcd.print("with good fats!");
        while (!ButtonPressed) {
          button();
        }
        if (ButtonPressed) {
          delay(3000);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("3. a cup of milk");
          lcd.setCursor(0, 1);
          lcd.print("Any milk works!");
          lcd.setCursor(0, 2);
          lcd.print("Soy, animal, etc.");
          while (!ButtonPressed) {
            button();
          }
          if (ButtonPressed) {
            delay(3000);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("4. Honey to taste");
            lcd.setCursor(0, 1);
            lcd.print("Everyone needs a");
            lcd.setCursor(0, 2);
            lcd.print("little sweetness");
            lcd.setCursor(0, 3);
            lcd.print("in their life!");
          }
        }
      }
    } else if (drinkSelection == 3) {
      lcd.print("1. a frozen banana");
      lcd.setCursor(0, 2);
      lcd.print("Bananas help you");
      lcd.setCursor(0, 3);
      lcd.print("with digestion!");
      while (!ButtonPressed) {
        button();
      }
      if (ButtonPressed) {
        delay(3000);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("2. a cup of");
        lcd.setCursor(0, 1);
        lcd.print("pineapple.");
        lcd.setCursor(0, 2);
        lcd.print("Pineapple gives you");
        lcd.setCursor(0, 3);
        lcd.print("lots of vitamin C!");
        while (!ButtonPressed) {
          button();
        }
        if (ButtonPressed) {
          delay(3000);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("3. a cup of milk");
          lcd.setCursor(0, 1);
          lcd.print("Any milk works!");
          lcd.setCursor(0, 2);
          lcd.print("but coconut milk");
          lcd.setCursor(0, 3);
          lcd.print("is recommended!");
          while (!ButtonPressed) {
            button();
          }
          if (ButtonPressed) {
            delay(3000);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("4. Honey to taste");
            lcd.setCursor(0, 1);
            lcd.print("Everyone needs a");
            lcd.setCursor(0, 2);
            lcd.print("little sweetness");
            lcd.setCursor(0, 3);
            lcd.print("in their life!");
          }
        }
      }
    } else if (drinkSelection == 4) {
      lcd.print("1. a frozen banana");
      lcd.setCursor(0, 2);
      lcd.print("Bananas help you");
      lcd.setCursor(0, 3);
      lcd.print("with digestion!");
      while (!ButtonPressed) {
        button();
      }
      if (ButtonPressed) {
        delay(3000);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("2. a cup of");
        lcd.setCursor(0, 1);
        lcd.print("strawberry.");
        lcd.setCursor(0, 2);
        lcd.print("Strawberries have");
        lcd.setCursor(0, 3);
        lcd.print("a lot of antioxidants");
        while (!ButtonPressed) {
          button();
        }
        if (ButtonPressed) {
          delay(3000);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("3. a cup of");
          lcd.setCursor(0, 1);
          lcd.print("blueberry.");
          lcd.setCursor(0, 2);
          lcd.print("Blueberries have");
          lcd.setCursor(0, 3);
          lcd.print("a lot of iron!");
          while (!ButtonPressed) {
            button();
          }
          if (ButtonPressed) {
            delay(3000);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("3. a cup of milk");
            lcd.setCursor(0, 1);
            lcd.print("Any milk works!");
            lcd.setCursor(0, 2);
            lcd.print("Soy, animal, etc.");
            while (!ButtonPressed) {
              button();
            }
            if (ButtonPressed) {
              delay(3000);
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("4. Honey to taste");
              lcd.setCursor(0, 1);
              lcd.print("Everyone needs a");
              lcd.setCursor(0, 2);
              lcd.print("little sweetness");
              lcd.setCursor(0, 3);
              lcd.print("in their life!");
            }
          }
        }
      }

    }
  }
}
void menuButton() {
  //if button is pressed, call mainMenu()
}

void mainMenu() {
  int select = 0;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Menu Selection");
  lcd.setCursor(0,1);
  lcd.print("1. Time");
  lcd.setCursor(0,2);
  lcd.print("2. Lights");
  lcd.setCursor(0,3);
  lcd.print("3. Reminders");
  lcd.setCursor(10,1);
  lcd.print("4. Stats");
  lcd.setCursor(10,2);
  lcd.print("5. Name");

  while (!ButtonPressed) {
    button();
    lcd.cursor();
    if (encoderPos == 1) {
      lcd.setCursor(0,1);
      select = 1;
    } else if (encoderPos == 2) {
      lcd.setCursor(0,2);
      select = 2;
    } else if (encoderPos == 3) {
      lcd.setCursor(0,3);
      select = 3;
    } else if (encoderPos == 4) {
      lcd.setCursor(10,1);
      select = 4;
    } else if (encoderPos == 5) {
      lcd.setCursor(10,2);
      select = 5;
    } else if (encoderPos >= 6) {
      encoderPos = 5;
    } else if (encoderPos == 255) {
      encoderPos = 1;
    }
  }
  if (ButtonPressed) {
    lcd.noCursor();
    buttonPressed = false;
    menu = select;
  }
}

void lightingControl() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("How would you like");
  lcd.setCursor(0, 1);
  lcd.print("the lighting to be?");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("1. Party Mode!!");
  lcd.setCursor(0,1);
  lcd.print("2. RGB Beam")
  lcd.setCursor(0,2);
  lcd.print("3. Breathe~")
  lcd.setCursor(0,3);
  lcd.print("4. Rainbow");
  while (!ButtonPressed) {
    button();
    lcd.setCursor(18, 3);
    if (encoderPos >= 5) {
      encoderPos = 4;
    } else if (encoderPos == 255) {
      encoderPos = 1;
    }
    lcd.print(encoderPos);
  }
  if (ButtonPressed) {
    int lightSelection = encoderPos;
    if(lightSelection==1){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Party mode it is!");
      lcd.setCursor(0,1);
      lcd.print("LET'S PARTY!!");
      lcd.setCursor(0,2);
      lcd.print("OwO");
      LED_PartyMode();
      delay(3000);
      lcd.clear();
    }else if(lightSelection==2){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("RGB Beam it is!");
      lcd.setCursor(0,1);
      lcd.print("red, green, blue...");
      LED_RGBBeam();
      delay(3000);
      lcd.clear();
    }else if(lightSelection==3){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Breathe it is~");
      lcd.setCursor(0,1);
      lcd.print("let's relax");
      lcd.setCursor(0,2);
      lcd.print("together~");
      LED_Breathe();
      delay(3000);
      lcd.clear();
    }else if(lightSelection==4){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Rainbow it is!");
      lcd.setCursor(0,1);
      lcd.print("7 colors!!");
      LED_Rainbow();
      delay(3000);
      lcd.clear();
    }
  }
}
void LED_PartyMode(int wait, int rainbowLoops, int whiteLoops){
int fadeVal=0, fadeMax=100;

    // Hue of first pixel runs 'rainbowLoops' complete loops through the color
    // wheel. Color wheel has a range of 65536 but it's OK if we roll over, so
    // just count from 0 to rainbowLoops*65536, using steps of 256 so we
    // advance around the wheel at a decent clip.
    for(uint32_t firstPixelHue = 0; firstPixelHue < rainbowLoops*65536;
      firstPixelHue += 256) {

      for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...

        // Offset pixel hue by an amount to make one full revolution of the
        // color wheel (range of 65536) along the length of the strip
        // (strip.numPixels() steps):
        uint32_t pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());

        // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
        // optionally add saturation and value (brightness) (each 0 to 255).
        // Here we're using just the three-argument variant, though the
        // second value (saturation) is a constant 255.
        strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue, 255,
          255 * fadeVal / fadeMax)));
      }

      strip.show();
      delay(wait);

      if(firstPixelHue < 65536) {                              // First loop,
        if(fadeVal < fadeMax) fadeVal++;                       // fade in
      } else if(firstPixelHue >= ((rainbowLoops-1) * 65536)) { // Last loop,
        if(fadeVal > 0) fadeVal--;                             // fade out
      } else {
        fadeVal = fadeMax; // Interim loop, make sure fade is at max
      }
    }

    for(int k=0; k<whiteLoops; k++) {
      for(int j=0; j<256; j++) { // Ramp up 0 to 255
        // Fill entire strip with white at gamma-corrected brightness level 'j':
        strip.fill(strip.Color(0, 0, 0, strip.gamma8(j)));
        strip.show();
      }
      delay(1000); // Pause 1 second
      for(int j=255; j>=0; j--) { // Ramp down 255 to 0
        strip.fill(strip.Color(0, 0, 0, strip.gamma8(j)));
        strip.show();
      }
    }

    delay(500); // Pause 1/2 second
}
void LED_RGBBeam(uint32_t color, int wait){
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}
void LEDBreathe(uint8_t wait){
  for(int j=0; j<256; j++) { // Ramp up from 0 to 255
    // Fill entire strip with white at gamma-corrected brightness level 'j':
    strip.fill(strip.Color(0, 0, 0, strip.gamma8(j)));
    strip.show();
    delay(wait);
  }

  for(int j=255; j>=0; j--) { // Ramp down from 255 to 0
    strip.fill(strip.Color(0, 0, 0, strip.gamma8(j)));
    strip.show();
    delay(wait);
  }
}
void LED_Rainbow(){
   if(whiteLength >= strip.numPixels()) whiteLength = strip.numPixels() - 1;

  int      head          = whiteLength - 1;
  int      tail          = 0;
  int      loops         = 3;
  int      loopNum       = 0;
  uint32_t lastTime      = millis();
  uint32_t firstPixelHue = 0;

  for(;;) { // Repeat forever (or until a 'break' or 'return')
    for(int i=0; i<strip.numPixels(); i++) {  // For each pixel in strip...
      if(((i >= tail) && (i <= head)) ||      //  If between head & tail...
         ((tail > head) && ((i >= tail) || (i <= head)))) {
        strip.setPixelColor(i, strip.Color(0, 0, 0, 255)); // Set white
      } else {                                             // else set rainbow
        int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
        strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
      }
    }

    strip.show(); // Update strip with new contents
    // There's no delay here, it just runs full-tilt until the timer and
    // counter combination below runs out.

    firstPixelHue += 40; // Advance just a little along the color wheel

    if((millis() - lastTime) > whiteSpeed) { // Time to update head/tail?
      if(++head >= strip.numPixels()) {      // Advance head, wrap around
        head = 0;
        if(++loopNum >= loops) return;
      }
      if(++tail >= strip.numPixels()) {      // Advance tail, wrap around
        tail = 0;
      }
      lastTime = millis();                   // Save time of last movement
    }
  }
}
void setupTime() {
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Setting Up Time!");
  month1 = 0;
  day1 = 0;
  year1 = 0;
  lcd.setCursor(3, 1);
  lcd.print("Choose Month:");
  lcd.setCursor(9, 3);
  lcd.print(timeNum);
  setupMonth();
  timeNum = 0;
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Setting Up Time!");
  lcd.setCursor(5, 1);
  lcd.print("Choose Day:");
  lcd.setCursor(9, 3);
  lcd.print(timeNum);
  setupDay();
  timeNum = 0;
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Setting Up Time!");
  lcd.setCursor(4, 1);
  lcd.print("Choose Year:");
  lcd.setCursor(8, 3);
  lcd.print(timeNum);
  setupYear();
  timeNum = 0;
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Setting Up Time!");
  lcd.setCursor(4, 1);
  lcd.print("Choose Hour:");
  lcd.setCursor(9, 3);
  lcd.print(timeNum);
  setupHour();
  timeNum = 0;
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Setting Up Time!");
  lcd.setCursor(5, 1);
  lcd.print("Choose Min:");
  lcd.setCursor(9, 3);
  lcd.print(timeNum);
  setupMin();
  timeNum = 0;
  setTime(hour1, min1, 30, day1, month1, year1);
}
void setupMonth() {
  month1 = 0;
  timeNum = 1;
  while (!ButtonPressed) {
    button();
    if (oldEncPos < encoderPos && timeNum < 12) {
      oldEncPos = encoderPos;
      timeNum++;
    } else if (oldEncPos > encoderPos && timeNum > 1) {
      oldEncPos = encoderPos;
      timeNum--;
    }
    lcd.setCursor(9, 3);
    lcd.print(timeNum);
    lcd.print(" ");
    //TODO: u can go past 12, doesnt show on arduino, but apparently it factors into return rotate #
  }
  ButtonPressed = false;
  month1 = timeNum;
  lcd.setCursor(4, 3);
  lcd.print("Selection: ");
  lcd.print(month1);
  Serial.println("Month Set: " + month1);
  delay(1500);
}
void setupDay() {
  day1 = 0;
  timeNum = 1;
  while (!ButtonPressed) {
    button();
    if (oldEncPos < encoderPos && timeNum < 31) {
      oldEncPos = encoderPos;
      timeNum++;
    } else if (oldEncPos > encoderPos && timeNum > 1) {
      oldEncPos = encoderPos;
      timeNum--;
    }
    lcd.setCursor(9, 3);
    lcd.print(timeNum);
    lcd.print(" ");
    //TODO: u can go past 31, doesnt show on arduino, but apparently it factors into return rotate #
  }
  ButtonPressed = false;
  day1 = timeNum;
  lcd.setCursor(4, 3);
  lcd.print("Selection: ");
  lcd.print(day1);
  Serial.println("Day Set: " + day1);
  delay(1500);
}
void setupYear() {
  year1 = 0;
  timeNum = 2021;
  while (!ButtonPressed) {
    button();
    if (oldEncPos < encoderPos && timeNum < 2100) {
      oldEncPos = encoderPos;
      timeNum++;
    } else if (oldEncPos > encoderPos && timeNum > 2021) {
      oldEncPos = encoderPos;
      timeNum--;
    }
    lcd.setCursor(8, 3);
    lcd.print(timeNum);
    lcd.print(" ");
    //TODO: u can go past 2100, doesnt show on arduino, but apparently it factors into return rotate #
  }
  ButtonPressed = false;
  year1 = timeNum;
  lcd.setCursor(2, 3);
  lcd.print("Selection: ");
  lcd.print(year1);
  Serial.println("Year Set: " + year1);
  delay(1500);
}
void setupHour() {
  hour1 = 0;
  timeNum = 1;
  while (!ButtonPressed) {
    button();
    if (oldEncPos < encoderPos && timeNum < 24) {
      oldEncPos = encoderPos;
      timeNum++;
    } else if (oldEncPos > encoderPos && timeNum > 1) {
      oldEncPos = encoderPos;
      timeNum--;
    }
    lcd.setCursor(9, 3);
    lcd.print(timeNum);
    lcd.print(" ");
    //TODO: u can go past 24, doesnt show on arduino, but apparently it factors into return rotate #
  }
  ButtonPressed = false;
  hour1 = timeNum;
  lcd.setCursor(4, 3);
  lcd.print("Selection: ");
  lcd.print(hour1);
  Serial.println("Hour Set: " + year1);
  delay(1500);
}
void setupMin() {
  min1 = 0;
  timeNum = 0;
  while (!ButtonPressed) {
    button();
    if (oldEncPos < encoderPos && timeNum < 60) {
      oldEncPos = encoderPos;
      timeNum++;
    } else if (oldEncPos > encoderPos && timeNum > 1) {
      oldEncPos = encoderPos;
      timeNum--;
    }
    lcd.setCursor(9, 3);
    lcd.print(timeNum);
    lcd.print(" ");
    //TODO: u can go past 60, doesnt show on arduino, but apparently it factors into return rotate #
  }
  ButtonPressed = false;
  min1 = timeNum;
  lcd.setCursor(4, 3);
  lcd.print("Selection: ");
  lcd.print(min1);
  Serial.println("Min Set: " + year1);
  delay(1500);
}
void displayTempTime() {
  //Sensor takes a bit to read sometimes, best to delay at 2
  delay(5000);
  // Read temperature as Celsius
  float c = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  float h = dht.readHumidity();
  Serial.print(c);
  Serial.print(f);
  if (!(isnan(c))) {
    pc = c;
  }
  if (!(isnan(f))) {
    pf = f;
  }
  String d = (String)month() + "/" + (String)day() + "/" + (String)year();
  //Time
  String t = (String)hour() + ":" + (String)minute(); //add +":"+(String)second() for seconds

  lcd.createChar(0, degree);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Date = ");
  lcd.print(d);
  lcd.setCursor(0, 1);
  lcd.print("Time = ");
  lcd.print(t);
  lcd.setCursor(0, 2);
  lcd.print("Temp = ");
  lcd.print((int)pf);
  lcd.write((byte)0);
  lcd.print("F/");
  lcd.print((int)pc);
  lcd.write((byte)0);
  lcd.print("C");
  lcd.setCursor(0, 3);
  lcd.print("Humidity = ");
  lcd.print((int)h * 0.1);
  lcd.print("%");
}
firstTime(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("|");
  lcd.setCursor(1, 0);
  lcd.print("Welcome to...");
  lcd.setCursor(19, 0);
  lcd.print("|");
  lcd.setCursor(0, 1);
  lcd.print("|");
  lcd.setCursor(1, 1);
  lcd.print("AuroraBot!!!");
  lcd.setCursor(19, 1);
  lcd.print("|");
  delay(5000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Initializing");
  lcd.setCursor(0, 1);
  lcd.print("your very own");
  lcd.setCursor(0, 2);
  lcd.print("AuroraBot......");
  delay(3000);
  setupTime();
}
void loop() {
  firstTime();
  /*
    if (menu == 1) {
     //Time Setup
     setupTime();
     menu++;
    } else if (menu == 2) {
     displayTempTime();
    } else if (menu == 3) {
    } else if (menu == 4) {
    } else if (menu == 5) {
    }*/
  daysActive++;
  if (i == 1) {
    morningCheckIn();
    i = 2;
  }
}

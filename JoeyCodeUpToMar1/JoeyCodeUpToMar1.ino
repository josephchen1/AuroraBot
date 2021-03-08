#include <LiquidCrystal_I2C.h>
#include <TimeLib.h>
#include <Vector.h>
static int pinA = 2;
static int pinB = 3;
static int CLK = 4;
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
int avgSleepTime = 0;
int totalSleepTime = 0;
int daysOfSleepTime = 0;
int goalSleepTime = 0;
LiquidCrystal_I2C lcd(0x27, 20, 4);
Vector<int> v;
void breathingExercise(){
  
}
void detoxDrink(){
  
}
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

//Don't change anything below
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
//Don't change anything above
void loop() {
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
    lcd.setCursor(0, 1);
    lcd.print("You slept for");
    lcd.setCursor(0, 2);
    lcd.print(sleepTime);
    lcd.setCursor(0, 3);
    lcd.print("hours");
    delay(3000);
    v.PushBack(sleepTime);
    daysOfSleepTime++;
    totalSleepTime += sleepTime;
    avgSleepTime = totalSleepTime / daysOfSleepTime;
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("You average sleep");
    lcd.setCursor(0, 2);
    lcd.print("time is " + avgSleepTime);
    delay(3000);
    if (avgSleepTime < goalSleepTime) {
      lcd.setCursor(0, 1);
      lcd.print("Oh no! Why did you");
      lcd.setCursor(0, 2);
      lcd.print("sleep less today?");
      lcd.setCursor(0, 3);
      lcd.print("1. Stress");
      lcd.setCursor(0, 4);
      lcd.print("2. Caffeine");
      lcd.setCursor(18, 4);
      while (!ButtonPressed) {
        button();
        lcd.setCursor(18, 3);
        if (encoderPos == 3) {
          encoderPos = 2;
        } else if (encoderPos == 255) {
          encoderPos = 1;
        }
        lcd.print(encoderPos);
      }
      if (ButtonPressed) {
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("You said that");
        lcd.setCursor(0, 2);
        if (encoderPos == 1) {
          lcd.print("stress is why you");
          lcd.setCursor(0, 3);
          lcd.print("didn't sleep so well");
          delay(3000);
          lcd.clear();
          lcd.setCursor(0, 1);
          lcd.print("Can we do some");
          lcd.setCursor(0, 2);
          lcd.print("breathing exercises?");
          lcd.setCursor(0, 3);
          lcd.print("to help with that?");
          while (!ButtonPressed) {
            button();
            lcd.setCursor(17, 4);
            if (encoderPos == 3) {
              encoderPos = 2;
            } else if (encoderPos == 255) {
              encoderPos = 1;
            }
            if (encoderPos == 1) {
              lcd.print("Yes");
            } else {
              lcd.print("No");
            }
            if (ButtonPressed) {
              if (encoderPos == 1) {
                lcd.clear();
                lcd.setCursor(0, 1);
                lcd.print("Great!");
                lcd.setCursor(0, 2);
                lcd.print("Let's destress!");
                lcd.setCursor(0, 3);
                lcd.print("Entering breathing");
                lcd.setCursor(0, 4);
                lcd.print("exercise...");
                delay(3000);
                lcd.clear();
                breathingExercise();
              }
            }
          }
        }
        if(encoderPos==2){
          lcd.print("caffeine is why you");
          lcd.setCursor(0, 3);
          lcd.print("didn't sleep so well");
          delay(3000);
          lcd.clear();
          lcd.setCursor(0, 1);
          lcd.print("Can we try and drink");
          lcd.setCursor(0, 2);
          lcd.print("something that's not");
          lcd.setCursor(0, 3);
          lcd.print("coffee to help?");
          while (!ButtonPressed) {
            button();
            lcd.setCursor(17, 4);
            if (encoderPos == 3) {
              encoderPos = 2;
            } else if (encoderPos == 255) {
              encoderPos = 1;
            }
            if (encoderPos == 1) {
              lcd.print("Yes");
            } else {
              lcd.print("No");
            }
            if (ButtonPressed) {
              if (encoderPos == 1) {
                lcd.clear();
                lcd.setCursor(0, 1);
                lcd.print("Great!");
                lcd.setCursor(0, 2);
                lcd.print("Let's detox!");
                lcd.setCursor(0, 3);
                lcd.print("Entering detox");
                lcd.setCursor(0, 4);
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

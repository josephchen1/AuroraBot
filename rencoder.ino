#include <TimeLib.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

int i = 1;
static int pinA = 2;
static int pinB = 3;
static int CLK = 4;
//Don't change anything below
volatile byte aFlag = 0;
volatile byte bFlag = 0;
volatile byte encoderPos = 0;
volatile byte oldEncPos = 0;
volatile byte reading = 0;
int timeNum = 1;
int month1;
int day1;
boolean selected = false;

int counter = 0;
int currentStateCLK;
int lastStateCLK;
String currentDir = "";
unsigned long lastButtonPress = 0;
float TimeNow2;
float TimeNow1;
boolean ButtonPressed = false;
LiquidCrystal_I2C lcd(0x27, 20, 4);
//Don't change anything above

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
  lcd.init();                      // initialize the lcd
  lcd.backlight();
  Serial.begin(9600);
  //Don't change anything above
}
void button() {
  int btnState = digitalRead(CLK);
  if (btnState == LOW) {
    if (millis() - lastButtonPress > 50) {
      //Serial.println("Button pressed!");
      ButtonPressed = true;
    }
    lastButtonPress = millis();
  }
  delay(100);
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
void setupTime() {
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Setting Up Time!");
  month1 = 0;
  day1 = 0;
  lcd.setCursor(3, 1);
  lcd.print("Choose Month:");
  lcd.setCursor(9, 3);
  lcd.print(timeNum);
  setupMonth();
}

void setupMonth() {
  month1 = 0;
  while(!ButtonPressed){
      button();
      if(oldEncPos < encoderPos && timeNum < 12){
        oldEncPos = encoderPos;
        timeNum++;
      } else if(oldEncPos > encoderPos && timeNum > 1){
        oldEncPos = encoderPos;
        timeNum--;
      }
      lcd.setCursor(9, 3);
      lcd.print(timeNum);
      lcd.print(" ");
      //TODO: u can go past 12, doesnt show on arduino, but apparently it factors into return rotate #
   }

  ButtonPressed = false;
  Serial.println("done");
}
void loop() {
  if (i == 1) {
    setupTime();
  }
  i = 2;
    if(ButtonPressed){
      //Serial.print("Button Pressed");
    }
    if(oldEncPos != encoderPos) {
      if(encoderPos==25){
        encoderPos = 24;
        Serial.println("This is the end, please scroll the other side");
      }else if(encoderPos==255){
        encoderPos = 0;
        Serial.println("This is the end, please scroll the other side");
     }
      Serial.println(encoderPos);
      oldEncPos = encoderPos;
    }
}

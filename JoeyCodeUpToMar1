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
int i=1;
int counter = 0;
int currentStateCLK;
int lastStateCLK;
String currentDir ="";
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

void setup() {
  //Don't change anything below
  pinMode(pinA, INPUT_PULLUP); 
  pinMode(pinB, INPUT_PULLUP);
  pinMode(CLK, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2),PinA,FALLING);
  attachInterrupt(digitalPinToInterrupt(3),PinB,FALLING);
  attachInterrupt(digitalPinToInterrupt(4),button, FALLING);
  Serial.begin(9600);
  lastStateCLK = digitalRead(CLK);
  TimeNow1 = millis();
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
}
void button(){
  int btnState = digitalRead(CLK);

  //If we detect LOW signal, button is pressed
  if (btnState == LOW) {
    if (millis() - lastButtonPress > 50) {
      ButtonPressed = true;
      Serial.print("Button Pressed");
    }
    lastButtonPress = millis();
  }else{
    ButtonPressed = false;
  }
  delay(1);

}

//Don't change anything below
void PinA(){
  reading = PIND & 0xC;
  if(reading == B00001100 && aFlag) {
    encoderPos --; 
    bFlag = 0;
    aFlag = 0;
  }
  else if (reading == B00000100){
    bFlag = 1;
  }
}

void PinB(){
  reading = PIND & 0xC;
  if (reading == B00001100 && bFlag) {
    encoderPos ++;
    bFlag = 0;
    aFlag = 0;
  }
  else if (reading == B00001000){
    aFlag = 1; 
  }
}
//Don't change anything above
void loop(){
  button();
    lcd.print("How many hours");
    lcd.setCursor(0, 1);
    lcd.print("did you sleep");
    lcd.setCursor(0, 2);
    lcd.print("last night?");
    lcd.setCursor(0, 3);
    while(!ButtonPressed){
      button();
      lcd.setCursor(18, 3);
      if(encoderPos==25){
        encoderPos = 24;
      }else if(encoderPos==255){
        encoderPos = 0;
      }
      lcd.print(encoderPos);
    }
    if(ButtonPressed){
      int sleepTime = encoderPos;
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("You slept for");
      lcd.setCursor(0, 2);
      lcd.print(sleepTime);
      lcd.setCursor(0,3);
      lcd.print("hours");
      delay(3000);
      s.PushBack(sleepTime);
      daysOfSleepTime++;
      totalSleepTime += sleepTime;
      avgSleepTime = totalSleepTime/daysOFSleepTime;
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("You average sleep");
      lcd.setCursor(0, 2);
      lcd.print("time is " + avgSleepTime);
      
      if(avgSleepTime<goalSleepTime){
        
      }
    }
}

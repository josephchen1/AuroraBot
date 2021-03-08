#include <TimeLib.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

#define DHTPIN ?????
//TODO: Add pin #
#define DHTTYPE DHT11

//Custom degree character
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
int year1;
int hour1;
int min1;

int menu = 1;

boolean selected = false;

int counter = 0;
int currentStateCLK;
int lastStateCLK;
String currentDir = "";
unsigned long lastButtonPress = 0;
float TimeNow2;
float TimeNow1;
boolean ButtonPressed = false;

//Initialize LCD Screen
LiquidCrystal_I2C lcd(0x27, 20, 4);

//Initialize DHT Sensor
DHT dht(DHTPIN, DHTTYPE);

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
  lcd.init(); //Starting LCD
  lcd.backlight();
  dht.begin(); //Starting DHT Sensor
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
  setTime(hour1,min1,30,day1,month1,year1);
}

void setupMonth() {
  month1 = 0;
  timeNum = 1;
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
  month1 = timeNum;
  lcd.setCursor(4,3);
  lcd.print("Selection: ");
  lcd.print(month1);
  Serial.print("Month Set: ");
  Serial.println(month1);
  delay(1500);
}

void setupDay() {
  day1 = 0;
  timeNum = 1;
  while(!ButtonPressed){
      button();
      if(oldEncPos < encoderPos && timeNum < 31){
        oldEncPos = encoderPos;
        timeNum++;
      } else if(oldEncPos > encoderPos && timeNum > 1){
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
  lcd.setCursor(4,3);
  lcd.print("Selection: ");
  lcd.print(day1);
  Serial.print("Day Set: ");
  Serial.println(day1);
  delay(1500);
}

void setupYear() {
  year1 = 0;
  timeNum = 2021;
  while(!ButtonPressed){
      button();
      if(oldEncPos < encoderPos && timeNum < 2100){
        oldEncPos = encoderPos;
        timeNum++;
      } else if(oldEncPos > encoderPos && timeNum > 2021){
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
  lcd.setCursor(2,3);
  lcd.print("Selection: ");
  lcd.print(year1);
  Serial.print("Year Set: ");
  Serial.println(year1);
  delay(1500);
}

void setupHour() {
  hour1 = 0;
  timeNum = 1;
  while(!ButtonPressed){
      button();
      if(oldEncPos < encoderPos && timeNum < 24){
        oldEncPos = encoderPos;
        timeNum++;
      } else if(oldEncPos > encoderPos && timeNum > 1){
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
  lcd.setCursor(4,3);
  lcd.print("Selection: ");
  lcd.print(hour1);
  Serial.print("Hour Set: ");
  Serial.println(hour1);
  delay(1500);
}

void setupMin() {
  min1 = 0;
  timeNum = 0;
  while(!ButtonPressed){
      button();
      if(oldEncPos < encoderPos && timeNum < 60){
        oldEncPos = encoderPos;
        timeNum++;
      } else if(oldEncPos > encoderPos && timeNum > 1){
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
  lcd.setCursor(4,3);
  lcd.print("Selection: ");
  lcd.print(min1);
  Serial.print("Min Set: ");
  Serial.println(min1);
  delay(1500);
}

void displayTempTime() {
  //Sensor takes a bit to read sometimes, best to delay at 2
  delay(2000);
  // Read temperature as Celsius
  float c = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);
  //Date
  String d = (String)month()+"/"+(String)day()+"/"+(String)year();
  //Time
  String t = (String)hour()+":"+(String)minute(); //add +":"+(String)second() for seconds

  lcd.createChar(0, degree);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Date = ");
  lcd.print(d);
  lcd.setCursor(0,1);
  lcd.print("Time = ");
  lcd.print(t);
  lcd.setCursor(0,2);
  lcd.print("Temp = ");
  lcd.print((int)f);
  lcd.write((byte)0);
  lcd.print("F/");
  lcd.print((int)c);
  lcd.write((byte)0);
  lcd.print("C");
}

void menuButton() {
  //add code that checks if main menu button is pressed
  mainMenu();
}

void mainMenu() {
  //add code
}

void lightingControl() {
  //add code
}

void loop() {
  if (menu == 1) {
    //Time Setup
    setupTime();
    menu++;
  } else if (menu == 2) {
    displayTempTime();
  } else if (menu == 3) {
    
  } else if (menu == 4) {
    
  } else if (menu == 5) {
    
  }
  
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

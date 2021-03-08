#include <LiquidCrystal_I2C.h>
#include <TimeLib.h>
#include <Vector.h>
#include <DHT.h>//add lib

static int pinA = 2;
static int pinB = 3;
static int CLK = 4;
volatile byte aFlag = 0;
volatile byte bFlag = 0;
volatile byte encoderPos = 0;
volatile byte oldEncPos = 0;
volatile byte reading = 0;

#define DHTPIN A0
//TODO: Add pin #
#define DHTTYPE DHT11
float pf;
float pc;

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
int menu = 1;
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

boolean selected = false;
int daysActive = 0;
int avgSleepTime = 0;
int totalSleepTime = 0;
int goalSleepTime = 8;
int avgMidday = 0;
int totalMidday = 0;
int goalMidday = 8;

LiquidCrystal_I2C lcd(0x27, 20, 4);
DHT dht(DHTPIN, DHTTYPE);
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

void breathingExercise() {

}
void detoxDrink() {
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
      encoderPos = 0;
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
    lcd.print("You will need:");
    lcd.setCursor(0, 1);
    if (drinkSelection == 1) {
      lcd.print("1. a frozen banana");
      lcd.setCursor(0, 2);
      lcd.print("Bananas help you");
      lcd.setCursor(0, 3);
      lcd.print("with digestion!");
      while (!ButtonPressed) {
        button();
      }
      if (ButtonPressed) {
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

    } else if (drinkSelection == 3) {

    } else if (drinkSelection == 4) {

    }
  }
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
  lcd.print((int)pf);
  lcd.write((byte)0);
  lcd.print("F/");
  lcd.print((int)pc);
  lcd.write((byte)0);
  lcd.print("C");
  lcd.setCursor(0,3);
  lcd.print("Humidity = ");
  lcd.print((int)h*0.1);
  lcd.print("%");
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
//  daysActive++;
//  if (i == 1) {
//    setupTime();
//    i = 2;
//  }
  displayTempTime();
}

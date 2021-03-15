#include <LiquidCrystal_I2C.h>
#include <TimeLib.h>
#include <Vector.h>
#include <DHT.h>
#include <Adafruit_NeoPixel.h>
#define DHTPIN A0
#define DHTTYPE DHT11
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

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
int owo=1;//variable just for demoing
int counter = 0;
unsigned long lastButtonPress = 0;
float TimeNow1;
boolean ButtonPressed = false;

unsigned long lastButtonPressM = 0;
boolean ButtonPressedM = false;
//variables to set time
int timeNum = 1;
int month1;
int day1;
int year1;
int hour1;
int min1;
//variables to keep track of the data collected from check-ins
int daysActive = 1;
double avgSleepTime = 0;
int totalSleepTime = 0;
int goalSleepTime = 8;

double avgMidday = 0;
int totalMidday = 0;
int goalMidday = 8;

double avgNight = 0;
int totalNight = 0;
int goalNight = 7;
//variables to keep track of when the user wants to check in
int morningCheckInTime = 0;
int middayCheckInTime = 0;
int nightCheckInTime = 0;

boolean menuOn = false;
boolean justRestarted = true;

LiquidCrystal_I2C lcd(0x27, 20, 4);
DHT dht(DHTPIN, DHTTYPE);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(28, LED, NEO_GRB + NEO_KHZ800);
//vectors to store data from check ins
Vector<int> sleepData;
Vector<int> middayData;
Vector<int> nightData;
void setup()
{
  //the code that looks weird
  //(like the next three lines)
  //are all from external sources.
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
  #endif
  strip.begin();
  strip.show();
  strip.setBrightness(50);
  pinMode(pinA, INPUT_PULLUP);
  pinMode(pinB, INPUT_PULLUP);
  pinMode(CLK, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), PinA, FALLING);
  attachInterrupt(digitalPinToInterrupt(3), PinB, FALLING);
  attachInterrupt(digitalPinToInterrupt(4), button, FALLING);
  Serial.begin(9600);
  TimeNow1 = millis();
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
  randomSeed(analogRead(10));
}
//standard button stuff, copied from textbook
void button()
{
  int btnState = digitalRead(CLK);

  //If we detect LOW signal, button is pressed
  if (btnState == LOW)
  {
    if (millis() - lastButtonPress > 50)
    {
      ButtonPressed = true;
    }
    lastButtonPress = millis();
  }
  else
  {
    ButtonPressed = false;
  }
  delay(1);
}
//standard button stuff, copied from textbook
void menuButton()
{
  int btnStateM = digitalRead(MENUBTN);

  //If we detect LOW signal, button is pressed
  if (btnStateM == LOW)
  {
    if (millis() - lastButtonPressM > 50)
    {
      ButtonPressedM = true;
      if(i==1){
        i=2;//dunno why but buttonpressedM is true when you just restart
        //so this is to prevent that.
      }else{
        menuOn = !menuOn;
        if(menuOn){
          mainMenu();
        }else{
          return;
        }
      }
    }
    lastButtonPressM = millis();
  }
  else
  {
    ButtonPressedM = false;
  }
  delay(1);
}
//copied from web
void PinA()
{
  reading = PIND & 0xC;
  if (reading == B00001100 && aFlag)
  {
    encoderPos--;
    bFlag = 0;
    aFlag = 0;
  }
  else if (reading == B00000100)
  {
    bFlag = 1;
  }
}
//copied from web
void PinB()
{
  reading = PIND & 0xC;
  if (reading == B00001100 && bFlag)
  {
    encoderPos++;
    bFlag = 0;
    aFlag = 0;
  }
  else if (reading == B00001000)
  {
    aFlag = 1;
  }
}
//morning check in! this is really long
//because of all the text
void morningCheckIn()
{
  button();
  lcd.print("How many hours");
  lcd.setCursor(0, 1);
  lcd.print("did you sleep");
  lcd.setCursor(0, 2);
  lcd.print("last night?");
  lcd.setCursor(0, 3);
  while (!ButtonPressed)
  {
    //this is to keep the encoder
    //within a reasonable range.
    //for example: someone can't sleep for 25 hours a day
    button();
    lcd.setCursor(18, 3);
    if (encoderPos == 25)
    {
      encoderPos = 24;
    }
    else if (encoderPos == 255)
    {
      encoderPos = 0;
    }
    lcd.print(encoderPos);
  }
  if (ButtonPressed)
  {
    int sleepTime = encoderPos;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("You slept for");
    lcd.setCursor(0, 1);
    lcd.print(sleepTime);
    lcd.setCursor(0, 2);
    lcd.print("hours");
    delay(3000);
    //calculating average,
    //i only want to keep it
    //as an integer, no decimals
    sleepData.PushBack(sleepTime);
    totalSleepTime += sleepTime;
    avgSleepTime = (totalSleepTime / daysActive);
    avgSleepTime = (int) avgSleepTime;
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
    if (sleepTime < goalSleepTime)
    {
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
      while (!ButtonPressed)
      {
        button();
        lcd.setCursor(18, 3);
        if (encoderPos >= 3)
        {
          encoderPos = 2;
        }
        else if (encoderPos == 0)
        {
          encoderPos = 1;
        }
        lcd.print(encoderPos);
      }
      if (ButtonPressed)
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("You said that");
        lcd.setCursor(0, 1);
        if (encoderPos == 1)
        {
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
          while (!ButtonPressed)
          {
            button();
            lcd.setCursor(10, 3);
            if (encoderPos == 3)
            {
              encoderPos = 2;
            }
            else if (encoderPos == 255)
            {
              encoderPos = 1;
            }
            if (encoderPos == 1)
            {
              lcd.print("Yes");
            }
            else
            {
              lcd.print("Nah");
            }
          }
          if (ButtonPressed)
          {
            if (encoderPos == 1)
            {
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
              for(int i=1;i<3;i++){
                  LED_RGBBeam(strip.Color(255,   0,   0), 200); // Red
                  LED_RGBBeam(strip.Color(  0, 255,   0), 250); // Green
                  LED_RGBBeam(strip.Color(  0,   0, 255), 350); // Blue
              }
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("Yay! Hope you feel");
              lcd.setCursor(0,1);
              lcd.print("better now!");
              delay(3000);
              lcd.clear();
            }
          }
        }
        if (encoderPos == 2)
        {
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
          while (!ButtonPressed)
          {
            button();
            lcd.setCursor(10, 3);
            if (encoderPos == 3)
            {
              encoderPos = 2;
            }
            else if (encoderPos == 255)
            {
              encoderPos = 1;
            }
            if (encoderPos == 1)
            {
              lcd.print("Yes");
            }
            else
            {
              lcd.print("Nah");
            }
          }
          if (ButtonPressed)
          {
            if (encoderPos == 1)
            {
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
            }else if(encoderPos==2){
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Don't worry!");
              lcd.setCursor(0, 1);
              lcd.print("It is completely");
              lcd.setCursor(0, 2);
              lcd.print("understandable.");
              lcd.setCursor(0, 3);
              lcd.print("Have a good day!");
              delay(3000);
              lcd.clear();
            }
          }
        }
      }
    }
  }
}
//midday check in! this is really long
//because of all the text
void middayCheckIn()
{
  button();
  lcd.print("How was your");
  lcd.setCursor(0, 1);
  lcd.print("morning on a scale");
  lcd.setCursor(0, 2);
  lcd.print("of 1 to 10?");
  while (!ButtonPressed)
  {
    button();
    lcd.setCursor(10, 3);
    if (encoderPos >= 11)
    {
      encoderPos = 10;
    }
    else if (encoderPos == 255)
    {
      encoderPos = 1;
    }
    lcd.print(encoderPos);
  }
    if (ButtonPressed)
    {
      button();
      int middayRating = encoderPos;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("You gave the morning");
      lcd.setCursor(0, 1);
      lcd.print(encoderPos);
      lcd.setCursor(3, 1);
      lcd.print("out of 10");
      delay(3000);
      middayData.PushBack(middayRating);
      totalMidday += middayRating;
      avgMidday = (totalMidday / daysActive);
      avgMidday = (int) avgMidday;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("You average midday");
      lcd.setCursor(0, 1);
      lcd.print("rating is ");
      lcd.setCursor(10, 1);
      lcd.print(avgMidday);
      lcd.setCursor(12, 1);
      lcd.print("/10");
      delay(3000);
      lcd.clear();
      if (middayRating < goalMidday)
      {
        lcd.setCursor(0, 0);
        lcd.print(":( Sorry about that");
        lcd.setCursor(0, 1);
        lcd.print("Why was your");
        lcd.setCursor(0, 2);
        lcd.print("morning not good?");
        delay(3000);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("1. Tired");
        lcd.setCursor(0, 1);
        lcd.print("2. Unmotivated");
        lcd.setCursor(0, 2);
        lcd.print("3. I needed a break");
        button();
        while (!ButtonPressed)
        {
          button();
          lcd.setCursor(18, 3);
          if (encoderPos >= 4)
          {
            encoderPos = 3;
          }
          else if (encoderPos == 255)
          {
            encoderPos = 1;
          }
          lcd.print(encoderPos);
        }
        if (ButtonPressed)
        {
          lcd.clear();
          if (encoderPos == 1)
          {
            lcd.setCursor(0, 0);
            lcd.print("Aww T_T");
            lcd.setCursor(0, 1);
            lcd.print("Poor little thing,");
            lcd.setCursor(0, 2);
            lcd.print("you know what,");
            lcd.setCursor(0, 3);
            lcd.print("you should sleep!");
            delay(5000);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Let me set the");
            lcd.setCursor(0, 1);
            lcd.print("light to relax mode");
            lcd.setCursor(0, 2);
            lcd.print("Can I trust you");
            lcd.setCursor(0, 3);
            lcd.print("that you will sleep?");
            delay(3000);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("[AuroraBot watches");
            lcd.setCursor(0,1);
            lcd.print("you silently]");
            button();
            while (!ButtonPressed)
            {
              button();
              lcd.setCursor(10, 2);
              if (encoderPos == 3)
              {
                encoderPos = 2;
              }
              else if (encoderPos == 255)
              {
                encoderPos = 1;
              }
              if (encoderPos == 1)
              {
                lcd.print("Yes");
              }
              else
              {
                lcd.print("Meh");
              }
            }
            if(ButtonPressed){
              lcd.clear();
              if(encoderPos==1){
                lcd.setCursor(0,0);
                lcd.print("Yay! Taking a nap");
                lcd.setCursor(0,1);
                lcd.print("is the best thing");
                lcd.setCursor(0,2);
                lcd.print("in the world!");
                lcd.setCursor(0,3);
                lcd.print("Good night!");
                for(int loop = 1;loop<300;loop++){
                  LED_RGBBeam(strip.Color(255,   0,   0), 200); // Red
                  LED_RGBBeam(strip.Color(  0, 255,   0), 200); // Green
                  LED_RGBBeam(strip.Color(  0,   0, 255), 200); // Blue
                }
              }else if(encoderPos==2){
                lcd.setCursor(0,0);
                lcd.print("You should try");
                lcd.setCursor(0,1);
                lcd.print("napping sometime");
                lcd.setCursor(0,2);
                lcd.print("but if it's not now");
                lcd.setCursor(0,3);
                lcd.print("that's alright too!");
              }
            }
          }
          else if (encoderPos == 2)
          {
            lcd.setCursor(0,0);
            lcd.print("Here is an");
            lcd.setCursor(0,1);
            lcd.print("inspirational");
            lcd.setCursor(0,2);
            lcd.print("quote, to cheer");
            lcd.setCursor(0,3);
            lcd.print("you up!");
            delay(5000);
            lcd.clear();
            lcd.setCursor(0,0);
            int randomQuote = random(1,5+1);
            if(randomQuote==1){
              lcd.print("All our dreams can");
              lcd.setCursor(0,1);
              lcd.print("come true, if we");
              lcd.setCursor(0,2);
              lcd.print("have the courage to");
              lcd.setCursor(0,3);
              lcd.print("pursue them! OwO");
            }else if(randomQuote==2){
              lcd.print("The secret of");
              lcd.setCursor(0,1);
              lcd.print("getting ahead is");
              lcd.setCursor(0,2);
              lcd.print("getting started");
              lcd.setCursor(10,3);
              lcd.print("@w@");
            }else if(randomQuote==3){
              lcd.print("The best time to");
              lcd.setCursor(0,1);
              lcd.print("plant a tree was 20");
              lcd.setCursor(0,2);
              lcd.print("years ago. The 2nd");
              lcd.setCursor(0,3);
              lcd.print("best time is now! :)");
            }else if(randomQuote==4){
              lcd.print("It’s hard to beat");
              lcd.setCursor(0,1);
              lcd.print("a person who");
              lcd.setCursor(0,2);
              lcd.print("never gives up");
              lcd.setCursor(10,3);
              lcd.print("QwQ");
            }else if(randomQuote==5){
              lcd.print("If we have the");
              lcd.setCursor(0,1);
              lcd.print("attitude that it's");
              lcd.setCursor(0,2);
              lcd.print("going to be a great");
              lcd.setCursor(0,3);
              lcd.print("day it usually is ;)");
            }
          }
          else if (encoderPos == 3)
          {
            lcd.setCursor(0,0);
            lcd.print("No problem! Taking");
            lcd.setCursor(0,1);
            lcd.print("a break is awesome!");
            lcd.setCursor(0,2);
            lcd.print("Have fun with the");
            lcd.setCursor(0,3);
            lcd.print("rest of your day!");
          }
          delay(5000);
          lcd.clear();
        }
      }else{
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Yay! AuroraBot is");
        lcd.setCursor(0,1);
        lcd.print("proud of you!");
        lcd.setCursor(0,2);
        lcd.print("Keep up the good");
        lcd.setCursor(0,3);
        lcd.print("work!");
        delay(5000);
        lcd.clear();
      }
    }
}
//night check in! this is really long
//because of all the text
void nightCheckIn()
{
  button();
  lcd.print("How was your");
  lcd.setCursor(0, 1);
  lcd.print("afternoon on a scale");
  lcd.setCursor(0, 2);
  lcd.print("of 1 to 10?");
  while (!ButtonPressed)
  {
    button();
    lcd.setCursor(10, 3);
    if (encoderPos >= 11)
    {
      encoderPos = 10;
    }
    else if (encoderPos == 255)
    {
      encoderPos = 1;
    }
    lcd.print(encoderPos);
  }
    if (ButtonPressed)
    {
      button();
      int nightRating = encoderPos;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("You gave the");
      lcd.setCursor(0, 1);
      lcd.print("afternoon a");
      lcd.setCursor(0,2);
      lcd.print(encoderPos);
      lcd.setCursor(3,2);
      lcd.print("out of 10");
      delay(3000);
      nightData.PushBack(nightRating);
      totalNight += nightRating;
      avgNight = (totalNight / daysActive);
      avgNight = (int) avgNight;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Your average");
      lcd.setCursor(0, 1);
      lcd.print("afternoon rating is");
      lcd.setCursor(0, 2);
      lcd.print(avgNight);
      lcd.setCursor(2, 2);
      lcd.print("out of 10");
      delay(3000);
      lcd.clear();
      if (nightRating < goalMidday)
      {
        lcd.setCursor(0, 0);
        lcd.print(":( Sorry about that");
        lcd.setCursor(0, 1);
        lcd.print("Why was your");
        lcd.setCursor(0, 2);
        lcd.print("afternoon not good?");
        delay(3000);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("1. Tired");
        lcd.setCursor(0, 1);
        lcd.print("2. Unmotivated");
        lcd.setCursor(0, 2);
        lcd.print("3. I took a break");
        button();
        while (!ButtonPressed)
        {
          button();
          lcd.setCursor(18, 3);
          if (encoderPos >= 4)
          {
            encoderPos = 3;
          }
          else if (encoderPos == 255)
          {
            encoderPos = 1;
          }
          lcd.print(encoderPos);
        }
        if (ButtonPressed)
        {
          lcd.clear();
          if (encoderPos == 1)
          {
            lcd.setCursor(0, 0);
            lcd.print("Well, it is the");
            lcd.setCursor(0, 1);
            lcd.print("evening right now.");
            lcd.setCursor(0, 2);
            lcd.print("You just need a");
            lcd.setCursor(0, 3);
            lcd.print("little final push!");
            delay(5000);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Let me set the");
            lcd.setCursor(0, 1);
            lcd.print("light to rainbow.");
            lcd.setCursor(0, 2);
            lcd.print("Will you try your");
            lcd.setCursor(0, 3);
            lcd.print("best to work?");
            delay(3000);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("[AuroraBot watches");
            lcd.setCursor(0,1);
            lcd.print("you with");
            lcd.setCursor(0,2);
            lcd.print("determination]");
            button();
            while (!ButtonPressed)
            {
              button();
              lcd.setCursor(2, 3);
              if (encoderPos == 3)
              {
                encoderPos = 2;
              }
              else if (encoderPos == 255)
              {
                encoderPos = 1;
              }
              if (encoderPos == 1)
              {
                lcd.print("Ohh yes!!!");
              }
              else
              {
                lcd.print("Not really");
              }
            }
            if(ButtonPressed){
              lcd.clear();
              if(encoderPos==1){
                lcd.setCursor(0,0);
                lcd.print("Yay! You are so");
                lcd.setCursor(0,1);
                lcd.print("close to the");
                lcd.setCursor(0,2);
                lcd.print("finish line!");
                lcd.setCursor(0,3);
                lcd.print("Good luck!");
                for(int loop=1;loop<100;loop++){
                  LED_Rainbow(75,5);
                }
              }else if(encoderPos==2){
                lcd.setCursor(0,0);
                lcd.print("Of course!");
                lcd.setCursor(0,1);
                lcd.print("You've worked so");
                lcd.setCursor(0,2);
                lcd.print("hard today already.");
                lcd.setCursor(0,3);
                lcd.print("That's alright too!");
              }
            }
          }
          else if (encoderPos == 2)
          {
            lcd.setCursor(0,0);
            lcd.print("Here is an");
            lcd.setCursor(0,1);
            lcd.print("inspirational");
            lcd.setCursor(0,2);
            lcd.print("quote, to cheer");
            lcd.setCursor(0,3);
            lcd.print("you up!");
            delay(5000);
            lcd.clear();
            lcd.setCursor(0,0);
            int randomQuote = random(1,5+1);
            if(randomQuote==1){
              lcd.print("All our dreams can");
              lcd.setCursor(0,1);
              lcd.print("come true, if we");
              lcd.setCursor(0,2);
              lcd.print("have the courage to");
              lcd.setCursor(0,3);
              lcd.print("pursue them! OwO");
            }else if(randomQuote==2){
              lcd.print("The secret of");
              lcd.setCursor(0,1);
              lcd.print("getting ahead is");
              lcd.setCursor(0,2);
              lcd.print("getting started");
              lcd.setCursor(10,3);
              lcd.print("@w@");
            }else if(randomQuote==3){
              lcd.print("The best time to");
              lcd.setCursor(0,1);
              lcd.print("plant a tree was 20");
              lcd.setCursor(0,2);
              lcd.print("years ago. The 2nd");
              lcd.setCursor(0,3);
              lcd.print("best time is now! :)");
            }else if(randomQuote==4){
              lcd.print("It’s hard to beat");
              lcd.setCursor(0,1);
              lcd.print("a person who");
              lcd.setCursor(0,2);
              lcd.print("never gives up");
              lcd.setCursor(10,3);
              lcd.print("QwQ");
            }else if(randomQuote==5){
              lcd.print("If we have the");
              lcd.setCursor(0,1);
              lcd.print("attitude that it's");
              lcd.setCursor(0,2);
              lcd.print("going to be a great");
              lcd.setCursor(0,3);
              lcd.print("day it usually is ;)");
            }
          }
          else if (encoderPos == 3)
          {
            lcd.setCursor(0,0);
            lcd.print("No problem! Taking");
            lcd.setCursor(0,1);
            lcd.print("a break is awesome!");
            lcd.setCursor(0,2);
            lcd.print("Have fun with the");
            lcd.setCursor(0,3);
            lcd.print("rest of your night!");
          }
          delay(5000);
          lcd.clear();
        }
      }else{
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Yay! AuroraBot is");
        lcd.setCursor(0,1);
        lcd.print("proud of you!");
        lcd.setCursor(0,2);
        lcd.print("Keep up the good");
        lcd.setCursor(0,3);
        lcd.print("work!");
        delay(5000);
        lcd.clear();
      }
    }
}
//the recipes are copied online
void detoxDrink(){
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
  while (!ButtonPressed)
  {
    button();
    lcd.setCursor(18, 3);
    if (encoderPos >= 5)
    {
      encoderPos = 4;
    }
    else if (encoderPos == 255)
    {
      encoderPos = 1;
    }
    lcd.print(encoderPos);
  }
  if (ButtonPressed)
  {
    int drinkSelection = encoderPos;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("For your drink");
    lcd.setCursor(0, 1);
    lcd.print("simply add all");
    lcd.setCursor(0, 2);
    lcd.print("ingredients together");
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
    if (drinkSelection == 1)
    {
      lcd.setCursor(0, 0);
      lcd.print("1. a frozen banana");
      lcd.setCursor(0, 1);
      lcd.print("Bananas help you");
      lcd.setCursor(0, 2);
      lcd.print("with digestion!");
      delay(3000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("2. a cup of spinach");
      lcd.setCursor(0, 1);
      lcd.print("Spinach benefits");
      lcd.setCursor(0, 2);
      lcd.print("your eye health!");
      delay(3000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("3. a cup of milk");
      lcd.setCursor(0, 1);
      lcd.print("Any milk works!");
      lcd.setCursor(0, 2);
      lcd.print("Soy, animal, etc.");
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
      delay(3000);
      lcd.clear();
    }else if (drinkSelection == 2)
    {
      lcd.print("1. a frozen banana");
      lcd.setCursor(0, 2);
      lcd.print("Bananas help you");
      lcd.setCursor(0, 3);
      lcd.print("with digestion!");
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
      delay(3000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("3. a cup of milk");
      lcd.setCursor(0, 1);
      lcd.print("Any milk works!");
      lcd.setCursor(0, 2);
      lcd.print("Soy, animal, etc.");
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
      delay(3000);
      lcd.clear();
    }
    else if (drinkSelection == 3)
    {
      lcd.print("1. a frozen banana");
      lcd.setCursor(0, 2);
      lcd.print("Bananas help you");
      lcd.setCursor(0, 3);
      lcd.print("with digestion!");
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
      delay(3000);
      lcd.clear();
    }
    else if (drinkSelection == 4)
    {
      lcd.print("1. a frozen banana");
      lcd.setCursor(0, 2);
      lcd.print("Bananas help you");
      lcd.setCursor(0, 3);
      lcd.print("with digestion!");
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
      delay(3000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("4. a cup of milk");
      lcd.setCursor(0, 1);
      lcd.print("Any milk works!");
      lcd.setCursor(0, 2);
      lcd.print("Soy, animal, etc.");
      delay(3000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("5. Honey to taste");
      lcd.setCursor(0, 1);
      lcd.print("Everyone needs a");
      lcd.setCursor(0, 2);
      lcd.print("little sweetness");
      lcd.setCursor(0, 3);
      lcd.print("in their life!");
      delay(3000);
      lcd.clear();
    }
  }
}
//joseph wrote this:
void mainMenu()
{
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
  lcd.print("5. Goals");

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
    ButtonPressed = false;
    if(select==1){
      button();
      setupTime();
    }else if(select==2){
      button();
      lightingControl();
    }else if(select==3){
      button();
      setupCheckIn();
    }else if(select==4){
      button();
      stats();
    }else if(select==5){
      button();
      setUpGoals();
    }
  }
}
//simple lighting control
//i have 4 options prepared!
int lightingControl()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("How would you like");
  lcd.setCursor(0, 1);
  lcd.print("the lighting to be?");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("1. Party Mode!!");
  lcd.setCursor(0, 1);
  lcd.print("2. RGB Beam");
  lcd.setCursor(0, 2);
  lcd.print("3. Rainbow");
  while (!ButtonPressed)
  {
    button();
    lcd.setCursor(18, 3);
    if (encoderPos >= 4)
    {
      encoderPos = 3;
    }
    else if (encoderPos == 255)
    {
      encoderPos = 1;
    }
    lcd.print(encoderPos);
  }
  if (ButtonPressed)
  {
    int lightSelection = encoderPos;
    if (lightSelection == 1)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Party mode it is!");
      lcd.setCursor(0, 1);
      lcd.print("LET'S PARTY!!");
      lcd.setCursor(0, 2);
      lcd.print("OwO");
      delay(3000);
      for(int loop=1;loop<300;loop++){
        LED_PartyMode(3);
      }

    }
    else if (lightSelection == 2)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("RGB Beam it is!");
      lcd.setCursor(0, 1);
      lcd.print("red, green, blue...");
      delay(3000);
      lcd.clear();
      for(int loop=1;loop<300;loop++){
        LED_RGBBeam(strip.Color(255,   0,   0), 50); // Red
        LED_RGBBeam(strip.Color(  0, 255,   0), 50); // Green
        LED_RGBBeam(strip.Color(  0,   0, 255), 50); // Blue
      }
    }
    else if (lightSelection == 3)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Rainbow it is!");
      lcd.setCursor(0, 1);
      lcd.print("7 colors!!");
      delay(3000);
      lcd.clear();
      for(int loop=1;loop<300;loop++){
        LED_Rainbow(75,5);
      }
    }
  }
}
//copied from web
void LED_RGBBeam(uint32_t color, int wait)
{
  for (int i = 0; i < strip.numPixels(); i++)
  {                                // For each pixel in strip...
    strip.setPixelColor(i, color); //  Set pixel's color (in RAM)
    strip.show();                  //  Update strip to match
    delay(wait);
  }
}
//copied from web
void LED_Rainbow(int whiteSpeed, int whiteLength)
{

  if (whiteLength >= strip.numPixels())
    whiteLength = strip.numPixels() - 1;

  int head = whiteLength - 1;
  int tail = 0;
  int loops = 3;
  int loopNum = 0;
  uint32_t lastTime = millis();
  uint32_t firstPixelHue = 0;

  for (;;)
  { // Repeat forever (or until a 'break' or 'return')
    for (int i = 0; i < strip.numPixels(); i++)
    {                                     // For each pixel in strip...
      if (((i >= tail) && (i <= head)) || //  If between head & tail...
          ((tail > head) && ((i >= tail) || (i <= head))))
      {
        strip.setPixelColor(i, strip.Color(0, 0, 0, 255)); // Set white
      }
      else
      { // else set rainbow
        int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
        strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
      }
    }

    strip.show(); // Update strip with new contents
    // There's no delay here, it just runs full-tilt until the timer and
    // counter combination below runs out.

    firstPixelHue += 40; // Advance just a little along the color wheel

    if ((millis() - lastTime) > whiteSpeed)
    { // Time to update head/tail?
      if (++head >= strip.numPixels())
      { // Advance head, wrap around
        head = 0;
        if (++loopNum >= loops)
          return;
      }
      if (++tail >= strip.numPixels())
      { // Advance tail, wrap around
        tail = 0;
      }
      lastTime = millis(); // Save time of last movement
    }
  }
}
//copied from web
void LED_PartyMode(int rainbowLoops)
{
  int fadeVal = 0, fadeMax = 100;

  // Hue of first pixel runs 'rainbowLoops' complete loops through the color
  // wheel. Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to rainbowLoops*65536, using steps of 256 so we
  // advance around the wheel at a decent clip.
  for (uint32_t firstPixelHue = 0; firstPixelHue < rainbowLoops * 65536;
       firstPixelHue += 256)
  {

    for (int i = 0; i < strip.numPixels(); i++)
    { // For each pixel in strip...

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

    if (firstPixelHue < 65536)
    { // First loop,
      if (fadeVal < fadeMax)
        fadeVal++; // fade in
    }
    else if (firstPixelHue >= ((rainbowLoops - 1) * 65536))
    { // Last loop,
      if (fadeVal > 0)
        fadeVal--; // fade out
    }
    else
    {
      fadeVal = fadeMax; // Interim loop, make sure fade is at max
    }
  }
}
//joseph wrote this
void setupTime()
{
  delay(1000);
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
//joseph wrote this
void setupMonth()
{
  month1 = 0;
  timeNum = 1;
  while (!ButtonPressed)
  {
    button();
    if (oldEncPos < encoderPos && timeNum < 12)
    {
      oldEncPos = encoderPos;
      timeNum++;
    }
    else if (oldEncPos > encoderPos && timeNum > 1)
    {
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
//joseph wrote this
void setupDay()
{
  day1 = 0;
  timeNum = 1;
  while (!ButtonPressed)
  {
    button();
    if (oldEncPos < encoderPos && timeNum < 31)
    {
      oldEncPos = encoderPos;
      timeNum++;
    }
    else if (oldEncPos > encoderPos && timeNum > 1)
    {
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
//joseph wrote this
void setupYear()
{
  year1 = 0;
  timeNum = 2021;
  while (!ButtonPressed)
  {
    button();
    if (oldEncPos < encoderPos && timeNum < 2100)
    {
      oldEncPos = encoderPos;
      timeNum++;
    }
    else if (oldEncPos > encoderPos && timeNum > 2021)
    {
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
//joseph wrote this
void setupHour()
{
  hour1 = 0;
  timeNum = 1;
  while (!ButtonPressed)
  {
    button();
    if (oldEncPos < encoderPos && timeNum < 24)
    {
      oldEncPos = encoderPos;
      timeNum++;
    }
    else if (oldEncPos > encoderPos && timeNum > 1)
    {
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
//joseph wrote this
void setupMin()
{
  min1 = 0;
  timeNum = 0;
  while (!ButtonPressed)
  {
    button();
    if (oldEncPos < encoderPos && timeNum < 60)
    {
      oldEncPos = encoderPos;
      timeNum++;
    }
    else if (oldEncPos > encoderPos && timeNum > 1)
    {
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
  return;
}
//joseph wrote this
void displayTempTime()
{
  //Sensor takes a bit to read sometimes, best to delay at 2
  delay(5000);
  // Read temperature as Celsius
  float c = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  float h = dht.readHumidity();
  Serial.print(c);
  Serial.print(f);
  if (!(isnan(c)))
  {
    pc = c;
  }
  if (!(isnan(f)))
  {
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
//initialization of bot
//asking for when the user wanna wake up,
//their goals, etc.
//and a cool welcome message hehe
void firstTime()
{
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
  setupCheckIn();
  setUpGoals();
}
//setting up the check in times
//can be accessed by user anytime via menu
void setupCheckIn(){
  delay(1000);
 lcd.clear();
 lcd.setCursor(0,0);
 lcd.print("When would you like");
 lcd.setCursor(0,1);
 lcd.print("to have your morning");
 lcd.setCursor(0,2);
 lcd.print("check in?");
 button();
  while (!ButtonPressed)
  {
    button();
    lcd.setCursor(9, 3);
    if (encoderPos == 24)
    {
      encoderPos = 23;
    }
    else if (encoderPos == 255)
    {
      encoderPos = 0;
    }
    lcd.print(encoderPos);
    //to make sure the user is not confused about
    //am or pm XD
    if(encoderPos<12){
      lcd.setCursor(11,3);
      lcd.print("a.m.");
    }else{
      lcd.setCursor(11,3);
      lcd.print("p.m.");
    }
  }
  if(ButtonPressed){
    morningCheckInTime=encoderPos;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Awesome!");
    lcd.setCursor(0,1);
    lcd.print("See you then!");
    delay(3000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("When would you like");
    lcd.setCursor(0,1);
    lcd.print("to have your midday");
    lcd.setCursor(0,2);
    lcd.print("check in?");
    button();
    while (!ButtonPressed)
    {
      button();
      lcd.setCursor(9, 3);
      if (encoderPos == 24)
      {
        encoderPos = 23;
      }
      else if (encoderPos == 255)
      {
        encoderPos = 0;
      }
      lcd.print(encoderPos);
      if(encoderPos<12){
        lcd.setCursor(11,3);
        lcd.print("a.m.");
      }else{
        lcd.setCursor(11,3);
        lcd.print("p.m.");
      }
    }
    if(ButtonPressed){
      middayCheckInTime=encoderPos;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Awesome!");
      lcd.setCursor(0,1);
      lcd.print("See you then!");
      delay(3000);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("When would you like");
      lcd.setCursor(0,1);
      lcd.print("to have your night");
      lcd.setCursor(0,2);
      lcd.print("check in?");
      button();
      while (!ButtonPressed)
      {
        button();
        lcd.setCursor(9, 3);
        if (encoderPos == 24)
        {
          encoderPos = 23;
        }
        else if (encoderPos == 255)
        {
          encoderPos = 0;
        }
        lcd.print(encoderPos);
        if(encoderPos<12){
          lcd.setCursor(11,3);
          lcd.print("a.m.");
        }else{
          lcd.setCursor(11,3);
          lcd.print("p.m.");
        }
      }if(ButtonPressed){
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Awesome!");
        delay(3000);
        lcd.clear();
        button();
      }
    }
  }
}
//setting up goals of check ins
//can be accessed by user anytime via menu
void setUpGoals(){
delay(1000);
 lcd.clear();
 lcd.setCursor(0,0);
 lcd.print("How many hours would");
 lcd.setCursor(0,1);
 lcd.print("you like to sleep?");
 delay(300);//uhhh
  button();
  while (!ButtonPressed)
  {
    button();
    lcd.setCursor(9, 2);
    if (encoderPos == 24)
    {
      encoderPos = 23;
    }
    else if (encoderPos == 255)
    {
      encoderPos = 0;
    }
    lcd.print(encoderPos);
    lcd.setCursor(11,2);
    lcd.print("hours");
  }
  if(ButtonPressed){
    goalSleepTime=encoderPos;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Awesome!");
    lcd.setCursor(0,1);
    lcd.print("That sounds like");
    lcd.setCursor(0,2);
    lcd.print("a good goal");
    delay(3000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("What is your goal");
    lcd.setCursor(0,1);
    lcd.print("for your midday");
    lcd.setCursor(0,2);
    lcd.print("rating?");
    button();
    while (!ButtonPressed)
    {
      button();
      lcd.setCursor(9, 3);
      if (encoderPos == 11)
      {
        encoderPos = 10;
      }
      else if (encoderPos == 255)
      {
        encoderPos = 0;
      }
      lcd.print(encoderPos);
    }
    if(ButtonPressed){
      goalMidday=encoderPos;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Awesome!");
      lcd.setCursor(0,1);
      lcd.print("Sounds like a good");
      lcd.setCursor(0,2);
      lcd.print("goal to achieve!");
      delay(3000);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("What is your goal");
      lcd.setCursor(0,1);
      lcd.print("for your night");
      lcd.setCursor(0,2);
      lcd.print("check in?");
      button();
      while (!ButtonPressed)
      {
        button();
        lcd.setCursor(9, 3);
        if (encoderPos == 11)
        {
          encoderPos = 10;
        }
        else if (encoderPos == 255)
        {
          encoderPos = 0;
        }
        lcd.print(encoderPos);
      }if(ButtonPressed){
        goalNight=encoderPos;
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Awesome!");
        lcd.setCursor(0,1);
        lcd.print("Sounds like a good");
        lcd.setCursor(0,2);
        lcd.print("goal to achieve!");
        delay(3000);
        button();
      }
    }
  }
}
//just a page to show all the data
//collected from check ins
void stats(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("avg. sleep:");
  lcd.setCursor(12,0);
  lcd.print(avgSleepTime);
  lcd.setCursor(14,0);
  lcd.print("hrs");
  lcd.setCursor(0,1);
  lcd.print("avg. mid. rate:");
  lcd.setCursor(16,1);
  lcd.print(avgMidday);
  lcd.setCursor(0,2);
  lcd.print("avg. night rate:");
  lcd.setCursor(17,2);
  lcd.print(avgNight);
  button();
  while (!ButtonPressed)
    {
      button();
    }
    if(ButtonPressed){
      lcd.clear();
    }
}
//check if a new day has passed
void newDay(){
 if(hour()==0&&minute()==0&&second()==0){
   daysActive++;
 }
}
//check if it is time to do check in
void timeForCheckIn(){
 if(hour()==morningCheckInTime){
   morningCheckIn();
 }
 else if(hour()==middayCheckInTime){
   middayCheckIn();
 }else if(hour()==nightCheckInTime){
   nightCheckIn();
 }
}
//check if aurora bot needs to be initialized
void isJustRestarted(){
   if(justRestarted){
    firstTime();
    justRestarted=false;
  }
}
void loop(){
 isJustRestarted();
 newDay();
 menuButton();
 timeForCheckIn();
}

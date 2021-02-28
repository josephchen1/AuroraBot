/*
 * Code from:
 * https://www.instructables.com/Improved-Arduino-Rotary-Encoder-Reading/
 */
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
int month;
boolean selected = false;


int counter = 0;
int currentStateCLK;
int lastStateCLK;
String currentDir ="";
unsigned long lastButtonPress = 0;
float TimeNow2;
float TimeNow1;
boolean ButtonPressed = false;
//Don't change anything above

void setup() {
  //Don't change anything below
  lcd.begin(20,4); // Initialize LCD
  pinMode(pinA, INPUT_PULLUP); 
  pinMode(pinB, INPUT_PULLUP);
  pinMode(CLK, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2),PinA,FALLING);
  attachInterrupt(digitalPinToInterrupt(3),PinB,FALLING);
  attachInterrupt(digitalPinToInterrupt(4),button, FALLING);
  Serial.begin(9600);
  lastStateCLK = digitalRead(CLK);
  TimeNow1 = millis();
  //Don't change anything above
}

void button(){
  int btnState = digitalRead(CLK);
  if (btnState == LOW) {
    if (millis() - lastButtonPress > 50) {
      Serial.println("Button pressed!");
      buttonPressed = true;
    }
    lastButtonPress = millis();
  }
  delay(1);
  buttonPressed = false;

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

void setupTime() {
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print(“Setting Up Time!”);
  lcd.setCursor(3,1);
  month = 0;
  day = 0;
  setupMonth();

void setupMonth() {
  while (!buttonPressed && !selected) {
  lcd.print(“Choose Month:”);
  lcd.setCursor(9,3);
  lcd.print(“”+timeNum);
  if (oldEncPos == 24 && encoderPos == 1) {
    timeNum++;
    if (buttonPressed) {
      if (timeNum > 0 && timeNum < 13) {
        month = timeNum;
        selected = true;
      }
      else {
        lcd.setCursor(2,3);
        lcd.print(“Incorrect Entry!”);
        setupMonth();
      }     
    }  
  }
  else if (oldEncPos == 1 && encoderPos == 24) {
    timeNum--;
    if (buttonPressed) {
      if (timeNum > 0 && timeNum < 13) {
        month = timeNum;
        selected = true;
      }
      else {
        lcd.setCursor(2,3);
        lcd.print(“Incorrect Entry!”);
        setupMonth();
      }       
    }
  }
  else if (oldEncPos < encoderPos) {
    timeNum++;
    if (buttonPressed) {
      if (timeNum > 0 && timeNum < 13) {
        month = timeNum;
        selected = true;
      }
      else {
        lcd.setCursor(2,3);
        lcd.print(“Incorrect Entry!”);
        setupMonth();
      }       
    }
  }
  else if (oldEncPos > encoderPos) {
    timeNum--;
    if (buttonPressed) {
      if (timeNum > 0 && timeNum < 13) {
        month = timeNum;
        selected = true;
      }
      else {
        lcd.setCursor(2,3);
        lcd.print(“Incorrect Entry!”);
        setupMonth();
      }       
    }
  }

  lcd.print(2,3);
  lcd.print(“Selection: “+month);
  delay(2000);
}
  
//Don't change anything above
void loop(){
  button();
  if(ButtonPressed){
    Serial.print("Button Pressed");
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

#include <Adafruit_NeoPixel.h>
#include <Wire.h>

const int LED_NUM = 12;
const int LED_PIN = 4;
const int HEART_BEAT_PIN = 5;

const float BRIGTHNESS_LOW = 0.33;
const float BRIGTHNESS_MID = 0.66;

const int THRESHOLD_GREEN = 50;
const int THRESHOLD_YELLOW = 70;
const int THRESHOLD_RED = 100;
 
struct RGB{
  byte r;
  byte g;
  byte b;
};

union DELAY{
  unsigned int value;
  byte bytes[4];  
};

union POWER{
  unsigned int value;
  byte bytes[4];  
};

//InitialValues
struct CONFIG_PARAMS {
  unsigned int rotation;
  unsigned int offset;
  unsigned int relayState;
  unsigned int personNear;
  unsigned int pattern_2;
  DELAY globalDelay;
};

unsigned int plugIsSelected = 0;

int startUp;
CONFIG_PARAMS start_values;
RGB color;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_NUM , LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(9600);      // initialize serial communication
  strip.begin();
  stripOff();              //Ensures that the strip is off when the plug is turned on
  strip.setBrightness(30);
  Wire.begin(8);                // join i2c bus with address #8
  Wire.onReceive(I2CValueRead); // register event
  blinkFourTime();
  startUp = 1;
  POWER measuredPower;
  pinMode(HEART_BEAT_PIN, OUTPUT);
}
 

void loop() {
    if(startUp == 0){
      if(start_values.rotation == 1){
        LedMotionSide1();
      }
      else{
        LedMotionSide2();
      }
    }
  delay(100); // DO NOT REMOVE THIS
}


/* Sets start up variables.*/
void startUpPlug(){
  while (Wire.available()) { 
    start_values.rotation = Wire.read();
    start_values.offset = Wire.read();
    start_values.relayState =Wire.read();
    start_values.personNear=Wire.read();
    start_values.pattern_2 = Wire.read();
    for(int i = 0; i <= 3; i++){
      start_values.globalDelay.bytes[i] = Wire.read();
    }
    Serial.print("The delay is ");
    Serial.print(start_values.globalDelay.value);
    
    if(start_values.personNear == 0){
      color = (RGB){0,0,0};                    // No color
    }
    else if(start_values.personNear == 1 && start_values.relayState == 0){
      color = (RGB){102, 0 ,102};             //Purple Color
    }
    else{
      color = (RGB){0,255,0};                 //Standart Color is always Green
    }
  }
  startUp = 0;          // Got the value to startup so no problem.
}


/* Gets the values from I2C and reads them.*/
void I2CValueRead(int howMany){   
 int startByte = Wire.read();
 Serial.print("This is my start byte");
 Serial.println(startByte);
 switch (startByte) {
   case (0):
     startUpPlug();
     break;
   case (1):
      POWER power;
     for(int i = 0; i <= 3; i++){
      power.bytes[i] = Wire.read();
     }
     colorChanger(power.value);
     break;
   case (2):
         start_values.relayState = Wire.read(); // receive byte
      break;
   case (3):
         start_values.rotation = Wire.read(); // receive byte
      break;
   case (4):
         start_values.personNear = Wire.read(); // receive byte
         Serial.println("Read Person Near");
         Serial.println(start_values.globalDelay.value);
         break;
   case (5):
         for(int i = 0; i <= 3; i++){
            start_values.globalDelay.bytes[i] = Wire.read();
         }
         break;
   case (6):
         plugIsSelected = Wire.read();
         color = (RGB){0,0,255};
         break;
   case (7):
         color = (RGB){0,0,0};
         startUp = 1;
         break;    
   default:
     ignoreSerie();
     break;
 }
 return;
}

/*Changes the color of the leds.*/
void colorChanger(int power){
  //Serial.println(power);
  if(start_values.personNear == 1 && start_values.relayState == 0){
    color =(RGB) {102, 0 ,102};           
  }else  if(power < THRESHOLD_GREEN &&  start_values.personNear == 1){
    color = (RGB){0, 255, 0};
  }
  else if( power < THRESHOLD_YELLOW &&  start_values.personNear == 1){
    color = (RGB){255, 255, 0};
  }
  else if(power > THRESHOLD_RED &&  start_values.personNear == 1){
    color = (RGB){255, 0, 0};
  }
  else{
    color = (RGB){0,0,0};
  }
}

/* Starts the motion */
void LedMotionSide1(){
  int i = start_values.offset;
  while(true){
    strip.setPixelColor((LED_NUM + i - 3)%LED_NUM,strip.Color(0,0,0)); 
    strip.setPixelColor((LED_NUM + i - 2)%LED_NUM,strip.Color(color.r*BRIGTHNESS_LOW ,color.g*BRIGTHNESS_LOW ,color.b*BRIGTHNESS_LOW)); 
    strip.setPixelColor((LED_NUM + i - 1)%LED_NUM,strip.Color(color.r*BRIGTHNESS_MID ,color.g*BRIGTHNESS_MID ,color.b*BRIGTHNESS_MID)); 
    strip.setPixelColor((LED_NUM + i)%LED_NUM,strip.Color(color.r,color.g,color.b)); 
    strip.setPixelColor((LED_NUM + i + start_values.pattern_2)%LED_NUM,strip.Color(color.r,color.g,color.b)); 
    strip.show();
    delay(start_values.globalDelay.value);
    strip.setPixelColor((LED_NUM + i + start_values.pattern_2)%LED_NUM,strip.Color(0,0,0)); 
    i++;
    if(i > LED_NUM - 1){
      i = 0;  
      digitalWrite(HEART_BEAT_PIN, HIGH);
    }
    if(i == 1){
      digitalWrite(HEART_BEAT_PIN, LOW);
    }
    Serial.println(start_values.rotation);
    if(start_values.rotation == 2 || startUp == 1){
      stripOff();
      break;
    }
  }
}

/* Starts the motion 
 */void LedMotionSide2(){
  int i = start_values.offset;
  while(true){
    if(i <= 0){
      i = 12;  
      digitalWrite(HEART_BEAT_PIN, HIGH);
    }
    strip.setPixelColor((i + 3)%LED_NUM,strip.Color(0,0,0));
    strip.setPixelColor((i + 2)%LED_NUM,strip.Color(color.r*BRIGTHNESS_LOW ,color.g*BRIGTHNESS_LOW ,color.b*BRIGTHNESS_LOW)); 
    strip.setPixelColor((i + 1)%LED_NUM,strip.Color(color.r*BRIGTHNESS_MID ,color.g*BRIGTHNESS_MID ,color.b*BRIGTHNESS_MID)); 
    strip.setPixelColor((i)%LED_NUM,strip.Color(color.r,color.g,color.b)); 
    //Plus two comes from the tail.
    strip.setPixelColor((i + 2 + start_values.pattern_2 )%LED_NUM,strip.Color(color.r,color.g,color.b)); 
    strip.show();
    delay(start_values.globalDelay.value);
    //Plus two comes from the tail.
    strip.setPixelColor((i + 2 + start_values.pattern_2)%LED_NUM,strip.Color(0,0,0)); 
    i--;
    if(i == 11){
      digitalWrite(HEART_BEAT_PIN, LOW);    
    }
    Serial.println(start_values.rotation);
    if(start_values.rotation == 1 || startUp == 1){
      stripOff();
      break;
    }
  }
}

/*Clean the LED color*/
void stripOff(){
  for(int i = 0; i < LED_NUM; i++){
    strip.setPixelColor(i, 0, 0, 0);  
    strip.show();
    }
}

/*Test the LED Colors*/
void blinkFourTime(){
  
  for(int j =0; j < 3; j++){
    if(j == 0){
      color = (RGB){255, 0, 0};
    }
    else if( j == 1)
    {
      color = (RGB){255, 255, 0};
    }
    else{
      color = (RGB){0, 255, 0};
    }
    for(int k =0; k <= 1; k++){
      for(int i = 0; i < LED_NUM; i++){
        strip.setPixelColor(i, color.r, color.g, color.b);  
       }    
       strip.show();
       delay(500);
       stripOff();
       delay(500);
    }
   }
 }

/*Read Garbadge Value FROM I2C*/
void ignoreSerie() {
 while (Wire.available()) { // loop through all
   Wire.read(); // receive byte
 }
}

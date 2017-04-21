#include <Adafruit_NeoPixel.h>
#include <Wire.h>

const int LED_NUM = 12;
const int LED_PIN = 4;

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

//InitialValues
struct CONFIG_PARAMS {
  int16_t rotation;
  int16_t delayTime;
  int16_t offset;
  int16_t relayState;
  int16_t personNear;
};

union POWER{
  int value;
  byte bytes[4];  
};

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
}
 

void loop() {
    //Serial.println("Waiting for I2C DATA");
    //Serial.println(startUp);
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


/* 
 *  Sets start up variables.
 *
 */
void startUpPlug(){
  while (Wire.available()) { 
    start_values = {Wire.read(), Wire.read(), Wire.read(), Wire.read(), Wire.read() };
    /*


int16_t rotation;
  int16_t delayTime;
  int16_t offset;
  int16_t relayState;
  int16_t personNear;
*/
    
    if(start_values.personNear == 0)
    {
      color = {0,0,0};        // No color
    }
    else if(start_values.personNear == 1 && start_values.relayState == 0)
    {
      color = {102, 0 ,102};  //Purple Color
    }
    else
    {
      color = {0,255,0};      //Standart Color is always Green
    }
    //Serial.print("Variable set");
    //Serial.println(startUp);
  }
  Serial.print("This is my delay value ");
    Serial.print(start_values.delayTime);
    Serial.println("");
     Serial.print("This is my Offset value ");
    Serial.print(start_values.offset);
    Serial.println();
     Serial.print("This is my relayState value ");
    Serial.print(start_values.relayState);
    Serial.println();
  startUp = 0;          // Got the value to startup so no problem.
}

/* Gets the values from I2C and reads them.
 */
void I2CValueRead(int howMany){   
 int startByte = Wire.read(); // receive first byte as an int
 Serial.print("This is my start byte");
 Serial.println(startByte);
 switch (startByte) {
   case (0):
     startUpPlug();
     break;
   case (1):
      POWER power;
      while (Wire.available()) { // loop through all
         for(int i = 0; i <= 3; i++){
            power.bytes[i] = Wire.read();
         }
       }
       //Serial.print("The values of the power are : ");
       //Serial.println(power.value);
     colorChanger(power.value);
   case (2):
      while (Wire.available()) { // loop through all
         start_values.relayState = Wire.read(); // receive byte
       }
      break;
   case (3):
      while (Wire.available()) { 
         start_values.rotation = Wire.read(); // receive byte
      }
      break;
   case (4):
      while (Wire.available()) { // loop through all
         start_values.personNear = Wire.read(); // receive byte
       }
   case (5):
      while (Wire.available()) { // loop through all
        start_values.delayTime = Wire.read(); // receive byte
        printf("The delay time is: ");
        printf(start_values.delayTime);
      }
   default:
     ignoreSerie();
     break;
 }
 return;
}


/**
 * Consumes one of the values
 */
void ignoreSerie() {
 while (Wire.available()) { // loop through all
   Wire.read(); // receive byte
 }
}


/*
 * Changes the color of the leds.
 */
void colorChanger(int power){
  //Serial.println(power);
  if(power < THRESHOLD_GREEN &&  start_values.personNear == 1){
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

/* Starts the motion 
 */
void LedMotionSide1(){
  int i = start_values.offset;
  bool firstPass = false;
  while(true){
    if(start_values.rotation == 2){
      break;
    }
    strip.setPixelColor(i,strip.Color(color.r,color.g,color.b)); 
    if(i > 0){
      strip.setPixelColor(i - 1,strip.Color(color.r*BRIGTHNESS_MID ,color.g*BRIGTHNESS_MID ,color.b*BRIGTHNESS_MID));  
    }
    if(i > 1){
      strip.setPixelColor(i - 1,strip.Color(color.r*BRIGTHNESS_MID ,color.g*BRIGTHNESS_MID ,color.b*BRIGTHNESS_MID));  
      strip.setPixelColor(i - 2,strip.Color(color.r*BRIGTHNESS_LOW ,color.g*BRIGTHNESS_LOW ,color.b*BRIGTHNESS_LOW));  
    }
    strip.show();
    delay(start_values.delayTime);
    if(i < LED_NUM - 1){
      if(i > 0){
        strip.setPixelColor(i - 1,strip.Color(color.r*BRIGTHNESS_MID ,color.g*BRIGTHNESS_MID ,color.b*BRIGTHNESS_MID));  
      }
      if(i > 1){
        strip.setPixelColor(i - 2,0,0,0);
        strip.setPixelColor(i - 1,0,0,0);  
      }
      strip.setPixelColor(i,strip.Color(color.r ,color.g ,color.b)); 
      strip.setPixelColor(i,0,0,0);  
      i++;
    }else{
      if(firstPass  == false ){
        firstPass = true;         
      }
      i = (i + 1)%LED_NUM;
    }
    if(firstPass == true && i == 0){
      strip.setPixelColor(LED_NUM - 3,0,0,0);  
    }
    if(firstPass == true && i == 1){
      strip.setPixelColor(LED_NUM - 2,0,0,0);  
    }
    if(firstPass == true && i == 2 ){
            strip.setPixelColor(LED_NUM - 1,0,0,0);  
      }
    strip.show();
  }
}
/* Starts the motion 
 */
void LedMotionSide2(){
  int i = start_values.offset;
  bool firstPass = false;
  while(true){
    if(start_values.rotation == 1){
      break;
    }
    strip.setPixelColor(i,strip.Color(color.r,color.g,color.b));
    if(i == 6 ){
      strip.setPixelColor(i + 1,strip.Color(color.r*BRIGTHNESS_MID ,color.g*BRIGTHNESS_MID ,color.b*BRIGTHNESS_MID));      
    } 
    if(i <= 5 ){
      strip.setPixelColor(i + 1,strip.Color(color.r*BRIGTHNESS_MID ,color.g*BRIGTHNESS_MID ,color.b*BRIGTHNESS_MID));      
      strip.setPixelColor(i + 2,strip.Color(color.r*BRIGTHNESS_LOW ,color.g*BRIGTHNESS_LOW ,color.b*BRIGTHNESS_LOW));      
    } 
    strip.show();
    delay(start_values.delayTime);
    if(i > 0){
      if(i == 6 && firstPass == false){
        strip.setPixelColor(i + 1,0,0,0);      
      } 
      if(i == 6 && firstPass == true){
        strip.setPixelColor(i + 1,0,0,0);      
      } 
      if(i <= 5 ){
        strip.setPixelColor(i + 1,0,0,0);      
        strip.setPixelColor(i + 2,0,0,0);      
      } 
    }else{
      firstPass = true;         
      i = LED_NUM;
    }
    //LEDs position are hard coded but this will be the same for LED Strips.
    if(i == LED_NUM && firstPass == true){
      strip.setPixelColor(2,0,0,0);      
    }
    if(i == LED_NUM - 1 && firstPass == true){
      strip.setPixelColor(1,0,0,0);      
    }
    if(i == LED_NUM - 2 && firstPass == true){
      strip.setPixelColor(0,0,0,0);      
    }
    strip.setPixelColor(i,0,0,0); 
    i = i - 1;
    
  }
}


/* Clean all the LED color
 */
void stripOff(){
  for(int i = 0; i < LED_NUM; i++){
    strip.setPixelColor(i, 0, 0, 0);  
    strip.show();
    }
}

/**
 * Test the LED Colors
 */
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

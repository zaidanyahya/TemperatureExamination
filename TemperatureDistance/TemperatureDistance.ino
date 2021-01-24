#include <M5Stack.h>
#include <Wire.h>
#include <Adafruit_AMG88xx.h>
#define echoPin 2 // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin 5 //attach pin D3 Arduino to pin Trig of HC-SR04

// Fonts
#include "Free_Fonts.h"

// Color
#define MAIN_COLOR 0x355C
#define ALLOWED_COLOR 0x4DA9
#define AMBIGUOUS_COLOR 0xEFE0
#define NOT_ALLOWED_COLOR 0xF269

Adafruit_AMG88xx amg;

float pixels[AMG88xx_PIXEL_ARRAY_SIZE];
unsigned int scale[15];

void setup() {
    M5.begin();
    M5.Power.begin();
    Serial.begin(115200);
    Serial.println(F("AMG88xx pixels"));

    bool status;
    
    // default settings
    status = amg.begin();
    if (!status) {
        Serial.println("Could not find a valid AMG88xx sensor, check wiring!");
        while (1);
    }

    //Speaker初期化
    M5.Speaker.begin();
    //変数初期化
    scale[0] = 261; //4ド C
    scale[1] = 293; //4レ D
    scale[2] = 329; //4ミ E
    scale[3] = 349; //4ファ F
    scale[4] = 391; //4ソ G
    scale[5] = 440; //4ラ A
    scale[6] = 493; //4シ B
    scale[7] = 523; //5ド
    scale[8] = 587; //5レ
    scale[9] = 659; //5ミ
    scale[10] = 698; //5ファ
    scale[11] = 783; //5ソ
    scale[12] = 880; //5ラ
    scale[13] = 987; //5シ
    scale[14] = 1046; //　6ド
    
    Serial.println("-- Pixels Test --");

    Serial.println();
    pinMode(5, OUTPUT); // Sets the trigPin as an OUTPUT
    pinMode(2, INPUT); // Sets the echoPin as an INPUT
    delay(100); // let sensor boot up
}


void loop() { 
    //read all the pixels
    amg.readPixels(pixels);

    int distance = calcDistance();
    if(distance){
      float data = pixels[19]+pixels[20]+pixels[21]+pixels[22];
      data += pixels[27]+2*pixels[28]+2*pixels[29]+pixels[30];
      data += pixels[35]+2*pixels[36]+2*pixels[37]+pixels[38];
      data += pixels[43]+pixels[44]+pixels[45]+pixels[46];
      
      Serial.print("Distance: ");
      Serial.print(distance);
      Serial.println(" cm");
      Serial.print("体温：");
      Serial.println(data/20);

      int j =  judge( data/20 , distance ) ;
      if( j == 1 ){
        allowedSound();
        allowedDisplay();
      }else if( j == 2){
        ambiguousSound();
        ambiguousDisplay();
      }else if( j == 3){
        notallowedSound();
        notallowedDisplay();
      }
      
    }
    
    //delay a second
    delay(1000);
}

int calcDistance(){
  // defines variables
  long duration; // variable for the duration of sound wave travel
  int distance; // variable for the distance measurement

  // Clears the trigPin condition
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  // Displays the distance on the Serial Monitor
  return distance;
}

int judge(float thermal, int distance){
  if(distance == 1){
    thermal += 5.772;
  }else if(distance == 2){
    thermal += 5.226;
  }else if(distance == 3){
    thermal += 7.198;
  }else if(distance == 4){
    thermal += 9.348;
  }else if(distance == 5 || (distance >= 5 && distance <= 10) ){
    thermal += 10.076;
  }else if(distance > 10){
    return 0;
  }

  if(thermal >= 35.0 && thermal <= 36.9){
    return 1;
  }else if(thermal <35.0 || (thermal >= 37.0 && thermal <= 37.4)){
    return 2;
  }else if(thermal > 37.5){
    return 3;
  }
}

void  allowedSound(){
  M5.Speaker.tone(scale[11]);
  delay(300);
  M5.Speaker.tone(scale[8]);
  delay(300);
  M5.Speaker.tone(scale[7]);
  delay(300);
  M5.Speaker.tone(scale[12]);
  delay(300);
  M5.Speaker.end();
}

void ambiguousSound(){
  M5.Speaker.tone(scale[12]);
  delay(300);
  M5.Speaker.tone(scale[11]);
  delay(300);
  M5.Speaker.tone(scale[8]);
  delay(300);
  M5.Speaker.tone(scale[7]);
  delay(300);
  M5.Speaker.end();
}

void notallowedSound(){
  M5.Speaker.tone(scale[14] + 30);
  delay(300);
  M5.Speaker.tone(scale[13] + 10);
  delay(300);
  M5.Speaker.tone(scale[12] + 25);
  delay(300);
  M5.Speaker.tone(scale[11] + 44);
  delay(300);
  M5.Speaker.end();
}

void allowedDisplay(){
  writeInCenter("OK", TFT_WHITE, ALLOWED_COLOR);
}

void ambiguousDisplay(){
  writeInCenter("RETRY", TFT_BLACK, AMBIGUOUS_COLOR);
}

void notallowedDisplay(){
  writeInCenter("GO HOME!", TFT_WHITE, NOT_ALLOWED_COLOR);
}

void writeInCenter(String text, int text_color, int bg_color){
  int xpos = 160;
  int ypos = 110;  
  M5.Lcd.fillScreen(bg_color);  
  M5.Lcd.setTextColor(text_color, bg_color);
  M5.Lcd.setFreeFont(FF19);    
  M5.Lcd.drawCentreString(text, xpos, ypos, GFXFF);
}

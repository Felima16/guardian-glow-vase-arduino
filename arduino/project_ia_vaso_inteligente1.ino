#include <Adafruit_NeoPixel.h>
#include <Servo.h>

#define PIN 2	 // input pin Neopixel is attached to

#define NUMPIXELS  8 // number of neopixels in strip

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


Servo myServo;
int codeSensorPins[]= {5,6,7,8};
int codeSensorStates[] = {LOW,LOW,LOW,LOW};
int sensorThreshold = HIGH;
int currentSensor =0;
int value[] = {0,0,0,0};
//
int servoPin = 13;
int servoOpen = 90;
int servoClosed= 0;
//
int redColor = 0;
int greenColor = 0;
int blueColor = 0;
int sensor = 3;
int val = 0;
int state = LOW;
int led = 4;
bool canEnterCode = false;




void setup() {
  // Initialize the NeoPixel library.
  pixels.begin();
  pinMode(sensor, INPUT);
  pinMode(led, OUTPUT);
  myServo.attach(servoPin);
  for (int i = 0; i < 4; i++) {
    pinMode(codeSensorPins[i], INPUT);
  }
  Serial.begin(9600);
}

void loop() {
  val  = digitalRead(sensor);
  //
  
   if(val == LOW){
    turnOff();
     delay(50);
    canEnterCode = false;
  }
 
  if(val == HIGH){
    turnOnIdle();
    digitalWrite(led, HIGH);
    delay(50);
    canEnterCode = true;
    delay(100);
    
  }
  
  while (canEnterCode == true){
    if(checkSequence () == true){
      turnOnSuccess();
      myServo.write(servoOpen);
      delay(2000);  // Wait for 2 seconds
      myServo.write(servoClosed);  // Close the door
      delay(2000);  // Wait for 2 seconds
      turnOnIdle();
      delay(2000);
      val == LOW;
      break;
      }
    if(checkSequence () == false){
      delay(100);
    }
  } 
}



void turnOnIdle(){
  for (int i=0; i < NUMPIXELS; i++) {
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, pixels.Color(redColor, greenColor , 255));
  }
  // This sends the updated pixel color to the hardware.
    pixels.show();
  }

void turnOff(){
  for (int i=0; i < NUMPIXELS; i++) {
    // Set RGB values to 0,0,0 to turn off the pixel
    pixels.setPixelColor(i, pixels.Color(0, 0, 0));
  }
  // This sends the updated pixel color to the hardware.
  pixels.show();
}

void turnOnSuccess(){
  for (int i=0; i < NUMPIXELS; i++) {
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, pixels.Color(redColor,  255, blueColor));
  }
  // This sends the updated pixel color to the hardware.
    pixels.show();
  }

void turnOnFail(){
  for (int i=0; i < NUMPIXELS; i++) {
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, pixels.Color(255, greenColor, blueColor));
  }
  // This sends the updated pixel color to the hardware.
    pixels.show();
  }
bool checkSequence(){
  int sensorOrder[] = {0, 1, 2, 3}; 
  int currentIndex = 0;
  bool sequenceCorrect = true;


  for (int i = 0; i < 4; i++) {
    value[i] = digitalRead(codeSensorPins[i]);
    codeSensorStates[i] = value [i];
    if (codeSensorStates[sensorOrder[currentIndex]] == HIGH) {
      Serial.print("Sensor ");
      Serial.print(sensorOrder[currentIndex]);
      Serial.println(" activated.");
      
      currentIndex++;

      // Check if currentIndex is out of bounds and reset it to 0
      if (currentIndex >= 4) {
        currentIndex = 0;
      }
    } else {
      // If the current sensor in the sequence is not HIGH, return false
      Serial.print("Sensor ");
      Serial.print(sensorOrder[currentIndex]);
      Serial.println(" not ativated");
      
      return false;
    }
    
    delay(3000); // Delay between sensor checks
  }

  Serial.println("Sequence Completed!");
  return true;
}
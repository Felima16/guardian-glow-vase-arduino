#include <CapacitiveSensor.h>

//Define capacitive sensors
CapacitiveSensor cs1 = CapacitiveSensor(8,9);
CapacitiveSensor cs2 = CapacitiveSensor(10,11);
CapacitiveSensor cs3 = CapacitiveSensor(12,13);

/******** colors workING **********
 * the process of RGB is inverted
 * so R 255 G 255 B 255 == black
 * and R 0 G 0 B 0 == White
 * Red:   R 0 / G 255 / B 255
 * Green: R 255 / G 0 / B 255
 * Blue:  R 255 / G 255 / B 0
*/

//Define pins PWM of the arduino to RGB control
#define R 3
#define G 2
#define B 4

//Sensor PIR defines
int pirPin = 6; // Input for HC-S501
int pirValue; // Place to store read PIR Value
bool isTurnOn = false;
bool isTryPassword = false;

//Define password sequence
int touch1 = 0;
int touch2 = 1;
int touch3 = 2;
int password[] = {1,2,0};
int sequence[3];
int indexSequence = 0;
int sequenceCount = 0;

int greenTons[] = {170,85,0};

void setup() {
  //Init monitoring
  Serial.begin(9600);

  //init sensor
  pinMode(pirPin, INPUT);

  //init LED strip - must be PWM to receive values between 0 and 255
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);

  //init capacitive sensor to touch
  cs1.set_CS_AutocaL_Millis(0xFFFFFFFF);
  cs2.set_CS_AutocaL_Millis(0xFFFFFFFF);
  cs3.set_CS_AutocaL_Millis(0xFFFFFFFF);
}

void loop() {
  // put your main code here, to run repeatedly:
  pirValue = digitalRead(pirPin);
  if(isTryPassword == false) {
    turningOnOffLight(pirValue);
  }
  
  verifyTouch();
}

// ***** FUNCTIONS *****

void turningOnOffLight(int pirValue) {
  if(pirValue == 1){
    analogWrite(R, 0);
    analogWrite(G, 0);
    analogWrite(B, 0);
    isTurnOn = true;
    Serial.println("Turn on light!!!!");
  } else {
    analogWrite(R, 255);
    analogWrite(G, 255);
    analogWrite(B, 255);
    isTurnOn = false;
  }
}

void verifyTouch() {
  long cs1Total = cs1.capacitiveSensor(500);
  long cs2Total = cs2.capacitiveSensor(500);
  long cs3Total = cs3.capacitiveSensor(500);
  
  if(cs1Total >= 1000) {
    Serial.println("Touch 1");
    sequence[indexSequence] = touch1;
    validatePassword();
  }

  if(cs2Total >= 1000) {
    Serial.println("Touch 2");
    sequence[indexSequence] = touch2;
    validatePassword();
  }

  if(cs3Total >= 1000) {
    Serial.println("Touch 3");
    sequence[indexSequence] = touch3;
    validatePassword();
  }
}

void validatePassword() {
  sequenceCount++;
  delay(500);
  bool validate = true;
  isTryPassword = true;
  for(int i=0; i<sequenceCount; i++) {
    if(password[i] != sequence[i]) {
      validate = false;
    }
  }

  if(validate == true) {
    showRightSequenceFeedback();
  } else {
    showWrongSequenceFeedback();
  }
  
}

void showRightSequenceFeedback() {
  Serial.println("showRightSequenceFeedback");
  analogWrite(R, 255);
  analogWrite(G, greenTons[indexSequence]);
  analogWrite(B, 255);
  indexSequence++;
}

void showWrongSequenceFeedback() {
  Serial.println("showWrongSequenceFeedback");
  analogWrite(R, 0);
  analogWrite(G, 255);
  analogWrite(B, 255);
  delay(2000);
  indexSequence = 0;
  sequenceCount = 0;
  isTryPassword = false;
  analogWrite(R, 255);
  analogWrite(G, 255);
  analogWrite(B, 255);
}

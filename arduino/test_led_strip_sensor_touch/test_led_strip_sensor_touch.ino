#include <Servo.h>

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
#define G 5
#define B 6
#define THRES 200
#define SAMPLES 20

typedef struct touchPad{
  int pin;
  int unpValue;
  int value;
  char state=0, prevState=0;
  char toggleState=0;
};

//Sensor PIR defines
int pirPin = 7; // Input for HC-S501
int pirValue; // Place to store read PIR Value
bool isTurnOn = false;
bool isTryPassword = false;

//Define password sequence
int touch1 = 0;
int touch2 = 1;
int touch3 = 2;
int touch4 = 3;

int password[] = {touch2,touch4,touch1,touch3};
int sequence[4];
int indexSequence = 0;
int sequenceCount = 0;

//RGB 
int greenTons[] = {192,126,63,0};
// variables to hold the LED color
int rVal = 254;
int gVal = 1;
int bVal = 127;

int rDir = -1;
int gDir = 1;
int bDir = -1;
bool isFade = false;

//strct sensor touch
touchPad touchPad1, touchPad2, touchPad3, touchPad4;

//Servo
Servo myservo;

//Time
long waitTouch = 5000;
unsigned long countTouch = 0;

void setup() {
  //Init monitoring
  Serial.begin(9600);

  /* Set A0 pull-up resistor, used to charge internal capacitor */
  pinMode(A0, INPUT_PULLUP);
  analogRead(A0);

  //init sensor
  pinMode(pirPin, INPUT);

  //init LED strip - must be PWM to receive values between 0 and 255
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);

 /* Initialize sensors */
  touchPadInit(&touchPad1, A1);
  touchPadInit(&touchPad2, A2);
  touchPadInit(&touchPad3, A3);
  touchPadInit(&touchPad4, A4);

  /* Initialize Servo */
  myservo.attach(9);
  myservo.write(0);
}

void loop() {
  // put your main code here, to run repeatedly:
  pirValue = digitalRead(pirPin);
  if(isTryPassword == false) {
    turningOnOffLight(pirValue);
  }

  if(isFade == true) {
    fade();
  }
  
  verifyTouch();
  verifyWaitTouch();
}


 /*******************************************************************************
 * #########################         FUNCTIONS        #########################
 *******************************************************************************/

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

void reset() {
  indexSequence = 0;
  sequenceCount = 0;
  isTryPassword = false;
  analogWrite(R, 255);
  analogWrite(G, 255);
  analogWrite(B, 255);
}

void rightPassword() {
  //fade in cena RGB
  openTheDoor();
  reset();
}

void verifyWaitTouch() {
  if((millis() - countTouch) > waitTouch){
    reset();
  }
}

 /*******************************************************************************
 * Validation
 *******************************************************************************/

void verifyTouch() {
 /* Scan sensors */
  touchPadScan(&touchPad1);
  touchPadScan(&touchPad2);
  touchPadScan(&touchPad3);
  touchPadScan(&touchPad4);

  if(touchPad1.state == 1) {
    Serial.println("Touch 1");
    sequence[indexSequence] = touch1;
    validatePassword();
  }

  if(touchPad2.state == 1) {
    Serial.println("Touch 2");
    sequence[indexSequence] = touch2;
    validatePassword();
  }

  if(touchPad3.state == 1) {
    Serial.println("Touch 3");
    sequence[indexSequence] = touch3;
    validatePassword();
  }

  if(touchPad4.state == 1) {
    Serial.println("Touch 4");
    sequence[indexSequence] = touch4;
    validatePassword();
  }
}

void validatePassword() {
  //Init wait touch
  countTouch = millis();
  isTurnOn == false;
  
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
    if(isPasswordFinish() == true) {
      rightPassword();
    }
    showRightSequenceFeedback();
  } else {
    showWrongSequenceFeedback();
  }
}

bool isPasswordFinish() {
  if(sequenceCount == 4){
    return true;
  }
  return false;
}

 /*******************************************************************************
 * RGB
 *******************************************************************************/

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
  reset();
}

void fade() {
  // PWM the LED
  // when using a common anode RGB LED like the ones in
  // your kits, you create a voltage difference across
  // each diode to light up the LED, that is, a PWM value
  // of 255 will turn that light off, while a PWM value of 0
  // will turn that light on fully.

  analogWrite(R, rVal);
  analogWrite(G, gVal);
  analogWrite(B, bVal);

  // change the values of the LEDs
  rVal = rVal + rDir;
  gVal = gVal + gDir;
  bVal = bVal + bDir;

  // for each color, change direction if
  // you reached 0 or 255
  if (rVal >= 255 || rVal <= 0) {
    rDir = rDir * -1;
  }

  if (gVal >= 255 || gVal <= 0) {
    gDir = gDir * -1;
  }

  if (bVal >= 255 || bVal <= 0) {
    bDir = bDir * -1;
  }

  // slight delay so it doesn't rotate color too quicky
  delay(33);
}


/*******************************************************************************
 * Servo
 *******************************************************************************/
void openTheDoor() {
  myservo.write(70);
  delay(10000);
  myservo.write(0);
}


 /*******************************************************************************
 * Touch sensor
 *******************************************************************************/

void touchPadInit(touchPad *pad, int pin){
  pad->pin=pin;
  pad->unpValue = (sampleB(pin) - sampleA(pin));
  DIDR0 |= 1;
  DIDR0 |= 1<<(pin-A0);
}

int sampleA(int sensePin){
  /* Sample capacitor is charged to VCC
   * via A0 pull-up resistor, touch pad
   * is discharged by pulling pin low
   */
  ADMUX = 0b01000000;
  pinMode(sensePin, OUTPUT);
  digitalWrite(sensePin, 0);
  
  pinMode(sensePin, INPUT);
  ADMUX = 0b01000000 | sensePin-A0;

  ADCSRA |= 1<<ADSC;
  while((ADCSRA & (1<<ADSC)) != 0);
  
  return ADC;
}

int sampleB(int sensePin){
  /* Sample capacitor is discharged by selecting
   * GND as ADC input, touch pad is charged to VCC
   * via pin pull-up resistor
   */
  ADMUX = 0b01001111;
  pinMode(sensePin, INPUT_PULLUP);
  
  pinMode(sensePin, INPUT);
  ADMUX = 0b01000000 | sensePin-A0;

  ADCSRA |= 1<<ADSC;
  while((ADCSRA & (1<<ADSC)) != 0);
  
  return ADC;
}

void touchPadScan(touchPad *pad){
  static float AA, BB;

  AA=0;
  BB=0;

  /* Get some readings from sensor and calculate average */
  for(int i=0; i<SAMPLES; i++){
    AA += sampleA(pad->pin);
    BB += sampleB(pad->pin);
  }

  AA /= SAMPLES;
  BB /= SAMPLES;
  
  pad->value = (BB - AA);

  /* Member unpValue is a running average of the unpressed readings.
   * A sudden change in sensor reading is interpreted as a touch
   */
  if(pad->value > (pad->unpValue + THRES))
    pad->state=1;
  else{
    pad->state=0;
    pad->unpValue=((float)pad->unpValue*0.9)+((float)pad->value*0.1);
  }
  
  if(pad->state == 1 && pad->prevState == 0)
    pad->toggleState = !pad->toggleState;

  pad->prevState = pad->state;
}

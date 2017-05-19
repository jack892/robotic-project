//#include <Wire.h>
#include <Servo.h>
#include <SparkFun_APDS9960.h>

// Pins
#define APDS9960_INT    3 // Needs to be an interrupt pin

#define MOTORLATCH 12
#define MOTORCLK 4
#define MOTORENABLE 7
#define MOTORDATA 8

#define MOTOR3_A 5
#define MOTOR3_B 7
#define MOTOR4_A 0
#define MOTOR4_B 6

#define MOTOR3_PWM 6
#define MOTOR4_PWM 5

#define FORWARD 1
#define BACKWARD 2
#define BRAKE 3
#define RELEASE 4

#define BEATFRICTION 75

// Constants

// Global Variables
SparkFun_APDS9960 apds = SparkFun_APDS9960();
uint8_t proximity_data = 0;

short speeds[] = {0, 50, 125, 200, 255};
short speedindex = 0;
short dirindex = 0;

enum MOVE { FWD, BWD, RGT, LFT };
MOVE STATUS = FWD;
int command = FORWARD;

int isr_flag = 0;

int speed = 50, rspeed = 0, lspeed = 0;

void setup() {

  // Use LED to signal that boot is over
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);

  // Set interrupt pin as input
  pinMode(APDS9960_INT, INPUT);

  // Initialize Serial port
  Serial.begin(115200);
  Serial.println();
  Serial.println("--------------------------------");
  Serial.println("SparkFun APDS-9960 - GestureTest");
  Serial.println("--------------------------------");

  // Initialize interrupt service routine
  attachInterrupt(APDS9960_INT, interruptRoutine, FALLING);

  // Initialize APDS-9960 (configure I2C and initial values)
  if ( apds.init() ) {
    Serial.println("APDS-9960 initialization complete");
  } else {
    Serial.println("Something went wrong during APDS-9960 init!");
  }

  // Start running the APDS-9960 gesture sensor engine
 if ( apds.enableGestureSensor(true) ) {
   Serial.println("Gesture sensor is now running");
 } else {
   Serial.println("Something went wrong during gesture sensor init!");
 }


}

void loop() {
 if (isr_flag == 1 ) {
    detachInterrupt(0);
    //Serial.println("call handleGesture");

    handleGesture();

    isr_flag = 0;
    attachInterrupt(APDS9960_INT, interruptRoutine, FALLING);

    delay(250);
  }
}

void interruptRoutine() {
  isr_flag = 1;
}

void movement(int command, int rspeed, int lspeed){
  motor(3, command, rspeed);
  motor(4, command, lspeed);
}
void invert(){
  if (command == FORWARD){
      command = BACKWARD;
      STATUS = BWD;
  }
  else if (command == BACKWARD){
      command = FORWARD;
      STATUS = FWD;
    }
  motor(3, RELEASE, 0);
  motor(4, RELEASE, 0);
}
void beat_friction(int index){
  if (index == 0) {
    movement(command, BEATFRICTION, BEATFRICTION);
  }
}
void handleGesture() {
    if ( apds.isGestureAvailable() ) {
    switch ( apds.readGesture() ) {
      case DIR_UP:
        Serial.print(STATUS);
        Serial.println(" -> UP");
        if (STATUS == FWD || STATUS == BWD){
          beat_friction(speedindex);
          speedindex = (speedindex<4)?++speedindex:speedindex;
          movement(command, speeds[speedindex],speeds[speedindex]);
          Serial.println("UP");
          Serial.println(speeds[speedindex]);
        }
        break;
      case DIR_DOWN:
        Serial.print(STATUS);
        Serial.println(" -> DOWN");
       if (STATUS == FWD || STATUS == BWD){
          if (speedindex == 0) {
            movement(command, BEATFRICTION, BEATFRICTION);
          }
          speedindex = (speedindex>0)?--speedindex:speedindex;
          movement(command, speeds[speedindex],speeds[speedindex]);
          Serial.println(speeds[speedindex]);
        }
        break;
      case DIR_LEFT:
        Serial.print(STATUS);
        Serial.println(" -> LEFT");
        if (STATUS == FWD || STATUS == BWD){
          STATUS = LFT;
          dirindex = speedindex - 1;
          movement(command, speeds[speedindex], speeds[dirindex]);
        }
        else if (STATUS == LFT && dirindex > 0){
          dirindex = 0;
          movement(command, speeds[speedindex], speeds[dirindex]);
        }
        else if (STATUS == RGT){
          if (command == FORWARD)
            STATUS = FWD;
          else
            STATUS = BWD;
          movement(command, BEATFRICTION,BEATFRICTION);
          movement(command, speeds[speedindex],speeds[speedindex]);
        }
        break;
      case DIR_RIGHT:
        Serial.print(STATUS);
        Serial.println(" -> RIGHT");
        if (STATUS == FWD || STATUS == BWD){
          STATUS = RGT;
          dirindex = speedindex - 1;
          movement(command, speeds[dirindex], speeds[speedindex]);
          Serial.println(speeds[dirindex]);
          Serial.println(speeds[speedindex]);
        }
        else if (STATUS == RGT && dirindex > 0){
          dirindex = 0;
          movement(command, speeds[dirindex], speeds[speedindex]);
        }
        else if (STATUS == LFT){
          if (command == FORWARD)
            STATUS = FWD;
          else
            STATUS = BWD;
          movement(command, BEATFRICTION,BEATFRICTION);
          movement(command, speeds[speedindex],speeds[speedindex]);
        }
        break;
      case DIR_NEAR:
        Serial.print(STATUS);
        Serial.println(" -> NEAR");
        if (STATUS == RGT) {
          movement(command, speeds[speedindex], speeds[dirindex]);
          STATUS = LFT;
        }
        else if (STATUS == LFT){
          movement(command, speeds[dirindex], speeds[speedindex]);
          STATUS = RGT;
        }
        else {
          invert();
          movement(command, BEATFRICTION,BEATFRICTION);
          movement(command, speeds[speedindex], speeds[speedindex]);
        }
        break;
      case DIR_FAR:
        Serial.print(STATUS);
        Serial.println(" -> FAR");
        if (STATUS == RGT) {
          movement(command, speeds[speedindex], speeds[dirindex]);
          STATUS = LFT;
        }
        else if (STATUS == LFT){
          movement(command, speeds[dirindex], speeds[speedindex]);
          STATUS = RGT;
        }
        else {
          invert();
          movement(command, BEATFRICTION,BEATFRICTION);
          movement(command, speeds[speedindex], speeds[speedindex]);
        }
        break;
      default:
        Serial.println("NONE");

    }
  }
}

void motor(int nMotor, int command, int speed)
{
int motorA, motorB;
if (nMotor >= 1 && nMotor <= 4)
{
switch (nMotor)
{
// case 1:
// motorA = MOTOR1_A;
// motorB = MOTOR1_B;
// break;
// case 2:
// motorA = MOTOR2_A;
// motorB = MOTOR2_B;
// break;
case 3:
motorA = MOTOR3_A;
motorB = MOTOR3_B;
break;
case 4:
motorA = MOTOR4_A;
motorB = MOTOR4_B;
break;
default:
break;
}
switch (command)
{
case FORWARD:
motor_output (motorA, HIGH, speed);
motor_output (motorB, LOW, -1); // -1: no PWM set
break;
case BACKWARD:
motor_output (motorA, LOW, speed);
motor_output (motorB, HIGH, -1); // -1: no PWM set
break;
case BRAKE:
motor_output (motorA, LOW, 255); // 255: fully on.
motor_output (motorB, LOW, -1); // -1: no PWM set
break;
case RELEASE:
motor_output (motorA, LOW, 0); // 0: output floating.
motor_output (motorB, LOW, -1); // -1: no PWM set
break;
default:
break;
}
}
}
void motor_output (int output, int high_low, int speed)
{
int motorPWM;
switch (output)
{
// case MOTOR1_A:
// case MOTOR1_B:
// motorPWM = MOTOR1_PWM;
// break;
// case MOTOR2_A:
// case MOTOR2_B:
// motorPWM = MOTOR2_PWM;
// break;
case MOTOR3_A:
case MOTOR3_B:
motorPWM = MOTOR3_PWM;
break;
case MOTOR4_A:
case MOTOR4_B:
motorPWM = MOTOR4_PWM;
break;
default:
speed = -3333;
break;
}
if (speed != -3333)
{
shiftWrite(output, high_low);
// set PWM only if it is valid
if (speed >= 0 && speed <= 255)
{
analogWrite(motorPWM, speed);
}
}
}
void shiftWrite(int output, int high_low)
{
static int latch_copy;
static int shift_register_initialized = false;
// Do the initialization on the fly,
// at the first time it is used.
if (!shift_register_initialized)
{
// Set pins for shift register to output
pinMode(MOTORLATCH, OUTPUT);
pinMode(MOTORENABLE, OUTPUT);
pinMode(MOTORDATA, OUTPUT);
pinMode(MOTORCLK, OUTPUT);
// Set pins for shift register to default value (low);
digitalWrite(MOTORDATA, LOW);
digitalWrite(MOTORLATCH, LOW);
digitalWrite(MOTORCLK, LOW);
// Enable the shift register, set Enable pin Low.
digitalWrite(MOTORENABLE, LOW);
// start with all outputs (of the shift register) low
latch_copy = 0;
shift_register_initialized = true;
}
// The defines HIGH and LOW are 1 and 0.
// So this is valid.
bitWrite(latch_copy, output, high_low);
shiftOut(MOTORDATA, MOTORCLK, MSBFIRST, latch_copy);
delayMicroseconds(5); // For safety, not really needed.
digitalWrite(MOTORLATCH, HIGH);
delayMicroseconds(5); // For safety, not really needed.
digitalWrite(MOTORLATCH, LOW);
}


//#include <Wire.h>
#include <Servo.h>
#include <SparkFun_APDS9960.h>

// pins
#define APDS9960_INT    3

// set pin for parallax ping
const int outPin = 2; // to use FAST_IO electrical resistance
const int inPin = 3; // pin 2 and 3 are usef for hight frequencies

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
#define BEATFRICTION 90

SparkFun_APDS9960 apds = SparkFun_APDS9960();
uint8_t proximity_data = 0;

long duration; // ping duration
long cm; // to convert the pulse from obstacle to centimeter
int obstacleDistanceCM = 60;

short speeds[] = {0, 50, 125, 200, 255};
short speedindex = 0;
short dirindex = 0;

enum MOVE {FWD, BWD, RGT, LFT};
MOVE STATUS = FWD;
int command = FORWARD;

int isr_flag = 0;

int speed = 50, rspeed = 0, lspeed = 0;

void setup() {
	// use led to signal that boot is over
	pinMode(13, OUTPUT);
	digitalWrite(13, HIGH);

	// set interrupt pin as input
	pinMode(APDS9960_INT, INPUT);

	// parallax ping
	pinMode(outPin, OUTPUT_FAST);
	pinMode(inPin, INPUT_FAST)

	// initialize serial port
	Serial.begin(115200);
	Serial.println();
	Serial.println("--------------------------------");
	Serial.println("SparkFun APDS-9960 - GestureTest");
	Serial.println("--------------------------------");

	// initialize interrupt service routine
	attachInterrupt(APDS9960_INT, interruptRoutine, FALLING);

	// initialize APDS-9960 (configure I2C and initial values)
	if (apds.init()) {
		Serial.println("APDS-9960 initialization complete");
	} else {
		Serial.println("Something went wrong during APDS-9960 init!");
	}

	// start running the APDS-9960 gesture sensor engine
	if (apds.enableGestureSensor(true)) {
		Serial.println("Gesture sensor is now running");
	} else {
		Serial.println("Something went wrong during gesture sensor init!");
	}
}


void loop() {
	if (isr_flag == 1 ) {
		detachInterrupt(0);
		handleGesture();

		isr_flag = 0;
		attachInterrupt(APDS9960_INT, interruptRoutine, FALLING);

		// obstacle detecting
		if (detectObstacle(obstacleDistanceCM) == 1) {
			escape();
		}

		speedindex = 0;
		delay(250);
	}
}

/*
 * Detect the obstacle.
 * Get from obstacleMaxDistance to the distance from the obstacle.
 */
int detectObstacle(int obstacleMaxDistance) {
	// the ping is actived from an hight frequency pulse (HIGH), therefore is necessary send a LOW pulse before in order to get an hight frequency pulse
	fastDigitalWrite(outPin, LOW);
	waitMicros(2);
	fastDigitalWrite(outPin, HIGH);
	waitMicros(10);
	fastDigitalWrite(outPin, LOW);

	// catch pulse thrown with hight frequency from the same pin
	duration = pulseIn(inPin, HIGH);

	// convert to centimeter
	cm = microsecondsToCentimeters(duration);

	Serial.print(cm);
	Serial.print("cm: ");
	Serial.println();

	if (cm > obstacleMaxDistance) {
		return 0;
		Serial.println("Free to go!");
	}
	else {
		return 1;
		Serial.println("Obstacle!");
	}
	Serial.println("\n\n\n\n\n\n\n\n\n\n");
}

/* Halt the sketck for for a specified microseconds amount */
void waitMicros(int val) {
	unsigned long a = micros();
	unsigned long b = micros();

	while ((b - a) < val) {
		b = micros();
		if (a > b) {
			break;
		}
	}
}

/*
 * Convert from time to distance
 * The sound speed is 340 m/s (29 microseconds per centimeter)
 * The ping pulse start from the device and come back, therefore
 * the distance is obtained by dividing for 2.
 */
long microsecondsToCentimeters(long microseconds) {
	return microseconds / 29 / 2;
}

void interruptRoutine() {
	isr_flag = 1;
}

void movement(int command, int rspeed, int lspeed) {
	motor(3, command, rspeed);
	motor(4, command, lspeed);
}

void invert() {
	if (command == FORWARD) {
		command = BACKWARD;
		STATUS = BWD;
	}
	else if (command == BACKWARD) {
		command = FORWARD;
		STATUS = FWD;
	}
	motor(3, RELEASE, 0);
	motor(4, RELEASE, 0);
}

void escape() {
	if (STATUS == FWD) {
		STATUS = LFT;
		dirindex = (speedindex < 4) ? speedindex + 1 : speedindex;
		speedindex = 4;
		command = BACKWARD;
		movement(command, speeds[dirindex], speeds[speedindex]);
		STATUS = BWD;
	}

	if (STATUS == RGT) {
		// command = BACKWARD;
		movement(command, speeds[speedindex], speeds[dirindex]);
		STATUS = RGT;
	}
	else if (STATUS == LFT) {
		// command = BACKWARD;
		movement(command, speeds[dirindex], speeds[speedindex]);
		STATUS = RGT;
	}
	if (STATUS == BWD) {
		invert();
		command = FORWARD;
		STATUS = FWD;
	}
	else {
		// invert();
		movement(command, BEATFRICTION, BEATFRICTION);
		movement(command, speeds[0], speeds[speedindex]);
	}
	// motor(3, RELEASE, 0);
	// motor(4, RELEASE, 0);
}

void beat_friction(int index) {
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
							if (STATUS == FWD || STATUS == BWD) {
								beat_friction(speedindex);
								speedindex = (speedindex < 4) ? ++speedindex : speedindex;
								movement(command, speeds[speedindex], speeds[speedindex]);
								Serial.println("UP");
								Serial.println(speeds[speedindex]);
							}
							break;
			case DIR_DOWN:
							Serial.print(STATUS);
							Serial.println(" -> DOWN");
							if (STATUS == FWD || STATUS == BWD) {
								if (speedindex == 0) {
									movement(command, BEATFRICTION, BEATFRICTION);
								}
								speedindex = (speedindex > 0) ? --speedindex : speedindex;
								movement(command, speeds[speedindex], speeds[speedindex]);
								Serial.println(speeds[speedindex]);
							}
							break;
			case DIR_LEFT:
							Serial.print(STATUS);
							Serial.println(" -> LEFT");
							if (STATUS == FWD || STATUS == BWD) {
								STATUS = LFT;
								dirindex = (speedindex < 4) ? speedindex + 1 : speedindex;
								movement(command, speeds[dirindex], speeds[speedindex]);
							}
							else if (STATUS == LFT && dirindex > 0) {
								dirindex = 0;
								movement(command, speeds[speedindex], speeds[dirindex]);
							}
							else if (STATUS == RGT) {
								if (command == FORWARD)
									STATUS = FWD;
								else
									STATUS = BWD;
								 movement(command, BEATFRICTION, BEATFRICTION);
								 movement(command, speeds[speedindex], speeds[speedindex]);
							}
							break;
			case DIR_RIGHT:
							Serial.print(STATUS);
							Serial.println(" -> RIGHT");
							if (STATUS == FWD || STATUS == BWD) {
								STATUS = RGT;
								dirindex = (speedindex < 4) ? speedindex + 1 : speedindex;
								movement(command, speeds[speedindex], speeds[dirindex]);
								Serial.println(speeds[dirindex]);
								Serial.println(speeds[speedindex]);
							}
							else if (STATUS == RGT && dirindex > 0) {
								dirindex = 0;
								movement(command, speeds[dirindex], speeds[speedindex]);
							}
							else if (STATUS == LFT) {
								if (command == FORWARD)
									STATUS = FWD;
								else
									STATUS = BWD;
								movement(command, BEATFRICTION, BEATFRICTION);
								movement(command, speeds[speedindex], speeds[speedindex]);
							}
							break;
			case DIR_NEAR:
							Serial.print(STATUS);
							Serial.println(" -> NEAR");
							if (STATUS == RGT) {
								movement(command, speeds[speedindex], speeds[dirindex]);
								STATUS = LFT;
							}
							else if (STATUS == LFT) {
								movement(command, speeds[dirindex], speeds[speedindex]);
								STATUS = RGT;
							}
							else {
								invert();
								movement(command, BEATFRICTION, BEATFRICTION);
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
							else if (STATUS == LFT) {
								movement(command, speeds[dirindex], speeds[speedindex]);
								STATUS = RGT;
							}
							else {
								invert();
								movement(command, BEATFRICTION, BEATFRICTION);
								movement(command, speeds[speedindex], speeds[speedindex]);
							}
							break;
			default: Serial.println("NONE");
		}
	}
}

void motor(int nMotor, int command, int speed) {
	int motorA, motorB;
	if (nMotor >= 1 && nMotor <= 4) {
		switch (nMotor) {
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
		switch (command) {
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
			default: break;
		}
	}
}

void motor_output (int output, int high_low, int speed) {
	int motorPWM;
	switch (output) {
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
	if (speed != -3333) {
		shiftWrite(output, high_low);
		// set PWM only if it is valid
		if (speed >= 0 && speed <= 255) {
			analogWrite(motorPWM, speed);
		}
	}
}

void shiftWrite(int output, int high_low) {
	static int latch_copy;
	static int shift_register_initialized = false;

	// do the initialization on the fly,
	// at the first time it is used.
	if (!shift_register_initialized) {
		// set pins for shift register to output
		pinMode(MOTORLATCH, OUTPUT);
		pinMode(MOTORENABLE, OUTPUT);
		pinMode(MOTORDATA, OUTPUT);
		pinMode(MOTORCLK, OUTPUT);
		// set pins for shift register to default value (low);
		digitalWrite(MOTORDATA, LOW);
		digitalWrite(MOTORLATCH, LOW);
		digitalWrite(MOTORCLK, LOW);
		// enable the shift register, set enable pin Low.
		digitalWrite(MOTORENABLE, LOW);
		// start with all outputs (of the shift register) low
		latch_copy = 0;
		shift_register_initialized = true;
	}
	// the defines high and low are 1 and 0.
	// so this is valid.
	bitWrite(latch_copy, output, high_low);
	shiftOut(MOTORDATA, MOTORCLK, MSBFIRST, latch_copy);
	delayMicroseconds(5); // for safety, not really needed.
	digitalWrite(MOTORLATCH, HIGH);
	delayMicroseconds(5); // for safety, not really needed.
	digitalWrite(MOTORLATCH, LOW);
}

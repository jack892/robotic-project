## Welcome to my robotic project

I've upgrade an old robotic project made by a group of guys for an university project, with the help of my co-worker [Alessandro](https://github.com/AMDeveloperC).

The main project is developed to be run on a microcontroller, Galileo v2 by Intel, the purpose was to drive a little car by hand gesture with the [SparkFun RGB and Gesture Sensor](https://www.sparkfun.com/products/12787).

Our upgrade consist to add a new feature to the little car, allow it to bypass an obstacle by itself. We used [PING)))™ ultrasonic sensor](https://www.parallax.com/product/28015), which it shoot an ultrasonic burst (well above human hearing) and then "listen" for the echo return pulse. The sensor measures the time required for the echo return,in this way we can calculate the distance between the sensor and the obstacle.

Below you can see the file we have written to configure the sensor and its functionality to merge at the main project and than adding the new feature.

########### sensor.ino ##########

const int outPin = 2; // using the  FAST_IO  
const int inPin = 3; //the pin 2 and 3 are used for high frequencies  

void setup() {  

  Serial.begin(115200);      
  pinMode(outPin, OUTPUT_FAST);  
  pinMode(inPin, INPUT_FAST);  
}  
  
void loop()  
{  
  // duration: the time required for the echo return,  
  // cm: distance in centimeters from the sensor to the obstacle.  
  long duration, cm;  
  
  // The PING device is activated by a high pulse frequency when it is set on HIGH  
  // so we switch back and forward between the low and high pulses,to assure that the sensore goes on HIGH pulse.  
  fastDigitalWrite(outPin, LOW);  
  waitMicros(2);  
  fastDigitalWrite(outPin, HIGH);  
  waitMicros(10);  
  fastDigitalWrite(outPin, LOW);  
  
  // Since we use the same pin to read the signal coming from the PING device  
   // we capture the high-frequency pulse coming from the same pin and we save it  
   // in the duration variable, which will contain the duration of the impulse in microseconds.  
  duration = pulseIn(inPin, HIGH);  
   
  // time to distance conversion  
  cm = microsecondsToCentimeters(duration);  
  
  //serial output  
  Serial.print(cm);  
  Serial.print("cm");  
  Serial.println();  
    
  delay(100);  
}  
  
/ function that set the process in waits for several microseconds, without interrupting the  
// running sketch  
void waitMicros(int val)  
{  
  unsigned long a = micros();  
  unsigned long b = micros();  
  while((b-a) < val)  
  {  
    b = micros();  
    if(a>b)  
    {  
      break;  
    }  
  }  
}  
  
//convertion of the time in distance  
long microsecondsToCentimeters(long microseconds)  
{  
  // The speed of sound is 340 meter/secs about  or 29 microseconds per centimeter.  
  // The sensor measures the time required when the ping it's shooted untill its echo returns back  
  // but in this way the distance it's misured in both ways instead just one way which it is the right   
  // distance, to avoid this little issue we just divide the total distance by 2.  
  return microseconds / 29 / 2;  
}  
  

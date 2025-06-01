#define STEERING_PIN 34
#define OUTPUT_PIN 5
#include <ESP32Servo.h>

Servo myservo;  // create servo object to control a servo
float dutyCycle0 = 0;
unsigned long highTime0 = 0;
unsigned long lowTime0 = 0;


void setup() {
  Serial.begin(115200);
  pinMode(STEERING_PIN, INPUT);
}

long dutyCycle() {
  // measures how long steering pin PWM is in HIGH or LOW
  highTime0 = pulseIn(STEERING_PIN, HIGH, 25000);

  lowTime0 = pulseIn(STEERING_PIN, LOW, 25000);


  // calculates the duty cycle 
  unsigned long period0 = highTime0 + lowTime0;
  dutyCycle0 = (highTime0 * 100.0) / period0;
}


void print_serial() {
  dutyCycle();
  Serial.print("_______STEERING______");
  Serial.print("High Time 0: ");
  Serial.print(highTime0);
  Serial.print(" us, Low Time 0: ");
  Serial.print(lowTime0);
  Serial.print(" us, Duty Cycle 0: ");
  Serial.print(dutyCycle0);
  Serial.println(" %");
  
}



// turn servo proportional to dutyCycle0
void turn_servo() {
  //9.825 for right, 5.175 for left, 7.5 = neutral, 2.325 shift.
  //60 degrees right, 60 degrees left
  //0.03875 shifts per degree
  //25.8064516129d degrees per shift
  const long MAX_DEGREE = 60;
  const long NEUTRAL = 7.5;
  const long SHIFT = 2.325;
  const long DEGREE_PER_SHIFT = MAX_DEGREE / SHIFT;

  dutyCycle();
  unsigned long shift_fraction = (dutyCycle0 - NEUTRAL) / SHIFT;

  long pos = shift_fraction * DEGREE_PER_SHIFT;
  myservo.write(pos);
  delay(10);

}




void loop() {
  print_serial();
  turn_servo();
  

  
}

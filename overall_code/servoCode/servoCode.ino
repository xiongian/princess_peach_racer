/*
 * https://circuits4you.com
 * ESP32 LED Blink Example
 * Board ESP23 DEVKIT V1
 * 
 * ON Board LED GPIO 2
 */

#include <ESP32Servo.h>
 
Servo myservo;  // create servo object to control a servo
// 16 servo objects can be created on the ESP32

const int pwmInputPin = 34;
unsigned long pulseWidth = 0;
unsigned long pulseWidth2 = 0;

int pos = 0;    // variable to store the servo position
// Recommended PWM GPIO pins on the ESP32 include 2,4,12-19,21-23,25-27,32-33 
int servoPin = 13;
 
void setup() {
	// Allow allocation of all timers
	Serial.begin(115200);
	ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
	myservo.setPeriodHertz(50);    // standard 50 hz servo
	myservo.attach(servoPin, 500, 2400); // attaches the servo on pin 18 to the servo object
	// using default min/max of 1000us and 2000us
	// different servos may require different min/max settings
	// for an accurate 0 to 180 sweep
}
 

 //
void loop() {
 
 
	for (pos = 0; pos <= 90; pos += 1) { // goes from 0 degrees to 180 degrees
		// in steps of 1 degree
		myservo.write(pos);    // tell servo to go to position in variable 'pos'
		delay(10);             // waits 15ms for the servo to reach the position
	}
	for (pos = 180; pos >= 90; pos -= 1) { // goes from 180 degrees to 0 degrees
		myservo.write(pos);    // tell servo to go to position in variable 'pos'
		delay(10);             // waits 15ms for the servo to reach the position
	}

	// Read length of HIGH pulse in microseconds
  pulseWidth = pulseIn(pwmInputPin, HIGH);
	pulseWidth2 = pulseIn(pwmInputPin, LOW);

///
	unsigned long highTime0 = pulseIn(pwmInputPin, HIGH);
  // Measure LOW duration
  unsigned long lowTime0 = pulseIn(pwmInputPin, LOW);

  // Calculate period and duty cycle
  unsigned long period0 = highTime0 + lowTime0;
  float dutyCycle0 = (highTime0 * 100.0) / period0;

	 Serial.print("test: ");
    Serial.print(highTime0);
		Serial.print(lowTime0);

///
  // Only show valid pulses (typically 1000–2000 us for RC)
  //if (pulseWidth > 800 && pulseWidth < 2200) {
    Serial.print("PWM Pulse Width: ");
    Serial.print(pulseWidth);
    Serial.println(" us");
		Serial.print("PWM Pulse Width: ");
    Serial.print(pulseWidth);
    Serial.println(" us");
  //}

  delay(10); // prevent spamming serial monitor

}
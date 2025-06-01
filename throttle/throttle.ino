# define PWM_PIN0 15
# define PWM_PIN1 4
# define OUTPUT_PIN 5
# define PWM_CHANNEL 0 
# define PWM_FREQ 50
# define PWM_RESOLUTION 16
 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(PWM_PIN0, INPUT);
  pinMode(PWM_PIN1, INPUT);
  pinMode(OUTPUT_PIN, OUTPUT);
  ledcAttach(OUTPUT_PIN, PWM_FREQ, PWM_RESOLUTION);
}

bool myLedcWrite(uint8_t pin, uint32_t dutyCycle){
  Serial.print("Output duty cycle: ");
  Serial.print(dutyCycle/65536.0*100.0);
  Serial.println(" %");
  return ledcWrite(pin, dutyCycle);
}

float lastDutyCycle0 = 7.5;
bool tone_avail = true;

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long highTime0 = pulseIn(PWM_PIN0, HIGH);
  // Measure LOW duration
  unsigned long lowTime0 = pulseIn(PWM_PIN0, LOW);

  // Calculate period and duty cycle
  unsigned long period0 = highTime0 + lowTime0;
  float dutyCycle0 = (highTime0 * 100.0) / period0;

  unsigned long highTime1 = pulseIn(PWM_PIN1, HIGH);
  unsigned long lowTime1 = pulseIn(PWM_PIN1, LOW);
  unsigned long period1 = highTime1 + lowTime1;
  float dutyCycle1 = (highTime1 * 100.0) / period1;

  Serial.print("High Time 0: ");
  Serial.print(highTime0);
  Serial.print(" us, Low Time 0: ");
  Serial.print(lowTime0);
  Serial.print(" us, Duty Cycle 0: ");
  Serial.print(dutyCycle0);
  Serial.println(" %");

  Serial.print("High Time 1: ");
  Serial.print(highTime1);
  Serial.print(" us, Low Time 1: ");
  Serial.print(lowTime1);
  Serial.print(" us, Duty Cycle 1: ");
  Serial.print(dutyCycle1);
  Serial.println(" %");

  // float forwardMultiplyFactor = 1.5;
  // float backwardMultiplyFactor = 1;

  // float offset = dutyCycle0 - 7.5;
  // if (offset > 0){
  //   offset *= forwardMultiplyFactor;
  // } else {
  //   offset *= backwardMultiplyFactor;
  // }

  // float offset = 0;
  // float finalDutyCycle0 = dutyCycle0 + offset;

  float dutyCycleChange0 = dutyCycle0-lastDutyCycle0;


  if (dutyCycle1 >4.9 && dutyCycle1 <5.1){ // if kill switch is on
    myLedcWrite(OUTPUT_PIN, (uint32_t)(7.5/100*65535));
    lastDutyCycle0 = 7.5;
    tone_avail = true;
  } else if (dutyCycle0>7.47 && dutyCycle0<7.53 && abs(dutyCycleChange0) < 0.5){
    myLedcWrite(OUTPUT_PIN, (uint32_t)(7.52/100*65535));
    if (tone_avail){
          tone(12,440,100);
          tone_avail = false;
    }
    lastDutyCycle0 = 7.52;
  } else if (dutyCycleChange0 > 0.5){ // if quick accel
    float overwriteDutyCycle0 = lastDutyCycle0 + 0.25;
    myLedcWrite(OUTPUT_PIN, (uint32_t)(overwriteDutyCycle0/100*65535));
    lastDutyCycle0 = overwriteDutyCycle0;
    tone_avail = true;
  } else if (dutyCycleChange0 < -0.5) { // if quick decel
    float overwriteDutyCycle0 = lastDutyCycle0 - 0.25;
    myLedcWrite(OUTPUT_PIN, (uint32_t)(overwriteDutyCycle0/100*65535));
    lastDutyCycle0 = overwriteDutyCycle0;
    tone_avail = true;
  } else {
    myLedcWrite(OUTPUT_PIN, dutyCycle0/100*65535);
    lastDutyCycle0 = dutyCycle0;
    tone_avail = true;
  }
  // delay(10);

}

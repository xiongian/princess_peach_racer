# define PWM_PIN0 15 // throttle input
# define PWM_PIN1 4 // kill switch input
# define PWM_PIN2 34 // steering input
# define OUTPUT_PIN0 5 // throttle output
# define OUTPUT_PIN1 19 // steering output

# define PWM_CHANNEL 0 
# define PWM_FREQ 50
# define PWM_RESOLUTION 16
# define BUZZER_PIN 12

bool myLedcWrite(uint8_t pin, uint32_t dutyCycle){
  Serial.print("Pin ");
  Serial.print(pin);
  Serial.print(" output duty cycle: ");
  Serial.print(dutyCycle/65536.0*100.0);
  Serial.println(" %");
  return ledcWrite(pin, dutyCycle);
}

// turn servo proportional to dutyCycle0
void turnServo(float dutyCycle) {
  //9.825 for right, 5.175 for left, 7.5 = neutral, 2.325 shift.
  //60 degrees right, 60 degrees left
  //0.03875 shifts per degree
  //25.8064516129d degrees per shift
  const long MAX_DEGREE = 60;
  const float NEUTRAL = 7.5;
  const float SHIFT = 2.325;
  const float DEGREE_PER_SHIFT = MAX_DEGREE / SHIFT;
  float result = dutyCycle + ((dutyCycle-7.5)/7.5*2.325);

  myLedcWrite(OUTPUT_PIN1,(uint32_t)(result/100.0*65535.0));

  // dutyCycle();
  // unsigned long shift_fraction = (dutyCycle - NEUTRAL) / SHIFT;

  // long pos = shift_fraction * DEGREE_PER_SHIFT;
  // myservo.write(pos);

  // delay(10);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(PWM_PIN0, INPUT);
  pinMode(PWM_PIN1, INPUT);
  pinMode(PWM_PIN2, INPUT);

  pinMode(OUTPUT_PIN0, OUTPUT);
  pinMode(OUTPUT_PIN1, OUTPUT);

  ledcAttachChannel(OUTPUT_PIN0, PWM_FREQ, PWM_RESOLUTION,1);
  ledcAttachChannel(BUZZER_PIN,440,4,2);
  ledcAttachChannel(OUTPUT_PIN1, PWM_FREQ, PWM_RESOLUTION,4);
}

float lastDutyCycle0 = 7.5;
bool tone_avail = true;
int state = 0;

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

  unsigned long highTime2 = pulseIn(PWM_PIN2, HIGH);
  unsigned long lowTime2 = pulseIn(PWM_PIN2, LOW);
  unsigned long period2 = highTime2 + lowTime2;
  float dutyCycle2 = (highTime2 * 100.0) / period2;

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

  Serial.print("High Time 1: ");
  Serial.print(highTime2);
  Serial.print(" us, Low Time 1: ");
  Serial.print(lowTime2);
  Serial.print(" us, Duty Cycle 1: ");
  Serial.print(dutyCycle2);
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

  turnServo(dutyCycle2);

  float dutyCycleChange0 = dutyCycle0-lastDutyCycle0;

  if (dutyCycleChange0>0.25){
    dutyCycleChange0 = 0.25;
  }
  if (dutyCycleChange0<-0.25){
    dutyCycleChange0 = -0.25;
  }

  float finalDutyCycle0 = lastDutyCycle0 + dutyCycleChange0;

  if(dutyCycle1<5.02 && dutyCycle1>4.98){
    finalDutyCycle0 = 7.5;
  }

  if (state==2 && finalDutyCycle0<7){
    finalDutyCycle0 = 7;
  }

  if (!tone_avail){
    myLedcWrite(BUZZER_PIN,0);
  }

  if (state==0 && tone_avail){
    myLedcWrite(BUZZER_PIN,16/2);
    tone_avail = false;
  }

  if (state!=0 && (finalDutyCycle0<7.45 || finalDutyCycle0>7.55)){
    tone_avail = true;
  }

  Serial.print("State: ");
  Serial.println(state);


  if (state==0){
    if (dutyCycleChange0>0.05){
      state=1;
    } else if (dutyCycleChange0<-0.05){
      state=3;
    }
  } else if (state==1 && finalDutyCycle0<7.45) {
    state = 2;
  } else if (state ==2 && finalDutyCycle0>7.45){
    state = 0;
  } else if (state==3){
    if (finalDutyCycle0>7.55){
      state = 1;
    } else if (finalDutyCycle0<7.55 && finalDutyCycle0>7.45 && tone_avail){
      myLedcWrite(BUZZER_PIN,16/2);
      tone_avail = false;
    }
  }

  myLedcWrite(OUTPUT_PIN0,(uint32_t)(finalDutyCycle0/100.0*65535.0));

  lastDutyCycle0 = finalDutyCycle0;

}

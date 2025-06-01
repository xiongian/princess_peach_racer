# define PWM_PIN0 15
# define PWM_PIN1 4
# define OUTPUT_PIN 5
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

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(PWM_PIN0, INPUT);
  pinMode(PWM_PIN1, INPUT);
  pinMode(OUTPUT_PIN, OUTPUT);
  ledcAttachChannel(OUTPUT_PIN, PWM_FREQ, PWM_RESOLUTION,1);
  ledcAttachChannel(BUZZER_PIN,440,4,2);
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

  myLedcWrite(OUTPUT_PIN,(uint32_t)(finalDutyCycle0/100.0*65535.0));

  lastDutyCycle0 = finalDutyCycle0;

}

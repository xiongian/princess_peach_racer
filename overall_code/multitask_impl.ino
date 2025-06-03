# define THROTTLE_INPUT 15 // throttle input
# define KILL_SWITCH 4 // kill switch input
# define STEERING_INPUT 34 // steering input
# define THROTTLE_OUTPUT 5 // throttle output
# define STEERING_OUTPUT 19 // steering output

# define PWM_CHANNEL 0 
# define PWM_FREQ 50
# define PWM_RESOLUTION 16
# define BUZZER_PIN 12

void myPrint(unsigned long ht, unsigned long lt, float dc){
  // Serial.printf("High time: %d Low time: %d Duty cycle %f\n", ht, lt, dc);
  Serial.print("High Time: ");
  Serial.print(ht);
  Serial.print(" us, Low Time: ");
  Serial.print(lt);
  Serial.print(" us, Duty Cycle: ");
  Serial.print(dc);
  Serial.println(" %");
  Serial.flush();
}

bool myLedcWrite(uint8_t pin, uint32_t dutyCycle){
  // Serial.print("Pin ");
  // Serial.print(pin);
  // Serial.print(" output duty cycle: ");
  // Serial.print(dutyCycle/65536.0*100.0);
  // Serial.println(" %");
  // Serial.flush();
  return ledcWrite(pin, dutyCycle);
}

void steeringControl(void *parameters){
  while(true){
    // Serial.println("steering");
    unsigned long highTime2 = pulseInLong(STEERING_INPUT, HIGH);
    unsigned long lowTime2 = pulseInLong(STEERING_INPUT, LOW);
    unsigned long period2 = highTime2 + lowTime2;
    float dutyCycle2 = (highTime2 * 100.0) / period2;
    float change = dutyCycle2-7.5;
    float a = 2;
    float k = 2.5/(pow(a,2.5)-1);
    if (change > 0){
      change = k * (pow(a,change)-1);
      dutyCycle2 = 7.5+change;
    } else{
      change = k * (pow(a,-change)-1);
      dutyCycle2 = 7.5-change;
    }
    // myPrint(highTime2, lowTime2, dutyCycle2);
    turnServo(dutyCycle2);
    delay(20 / portTICK_PERIOD_MS);
  }
}

// turn servo proportional to dutyCycle0
void turnServo(float dutyCycle) {
  //9.825 for right, 5.175 for left, 7.5 = neutral, 2.325 shift.
  //60 degrees right, 60 degrees left
  //0.03875 shifts per degree
  //25.8064516129d degrees per shift
  const long MAX_DEGREE = 30;
  // const float NEUTRAL = 7.5;
  // const float SHIFT = 2.325;
  // const float DEGREE_PER_SHIFT = MAX_DEGREE / SHIFT;
  float result = dutyCycle + ((dutyCycle-7.5)*(MAX_DEGREE/90.0)/7.5*2.325);

  myLedcWrite(STEERING_OUTPUT,(uint32_t)(result/100.0*65535.0));

  // dutyCycle();
  // unsigned long shift_fraction = (dutyCycle - NEUTRAL) / SHIFT;

  // long pos = shift_fraction * DEGREE_PER_SHIFT;
  // myservo.write(pos);

  // delay(10);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(THROTTLE_INPUT, INPUT);
  pinMode(KILL_SWITCH, INPUT);
  pinMode(STEERING_INPUT, INPUT);

  pinMode(THROTTLE_OUTPUT, OUTPUT);
  pinMode(STEERING_OUTPUT, OUTPUT);

  ledcAttachChannel(THROTTLE_OUTPUT, PWM_FREQ, PWM_RESOLUTION,1);
  ledcAttachChannel(BUZZER_PIN,440,4,2);
  ledcAttachChannel(STEERING_OUTPUT, PWM_FREQ, PWM_RESOLUTION,4);

  xTaskCreatePinnedToCore(
    steeringControl,    // Function that should be called
    "steering control",   // Name of the task (for debugging)
    4096,            // Stack size (bytes)
    NULL,            // Parameter to pass
    1,               // Task priority
    NULL,             // Task handle
    0
  );
  // xTaskCreatePinnedToCore(
  //   throttleControl,    // Function that should be called
  //   "throttle control",   // Name of the task (for debugging)
  //   4096,            // Stack size (bytes)
  //   NULL,            // Parameter to pass
  //   1,               // Task priority
  //   NULL,             // Task handle
  //   1
  // );
}

void loop() {
  float lastDutyCycle0 = 7.5;
  bool tone_avail = true;
  int state = 0;
  while (true) {
    // Serial.println("throttle");
    unsigned long highTime0 = pulseInLong(THROTTLE_INPUT, HIGH);
    unsigned long lowTime0 = pulseInLong(THROTTLE_INPUT, LOW);
    unsigned long period0 = highTime0 + lowTime0;
    float dutyCycle0 = (highTime0 * 100.0) / period0;

    unsigned long highTime1 = pulseInLong(KILL_SWITCH, HIGH);
    unsigned long lowTime1 = pulseInLong(KILL_SWITCH, LOW);
    unsigned long period1 = highTime1 + lowTime1;
    float dutyCycle1 = (highTime1 * 100.0) / period1;

    myPrint(highTime0, lowTime0, dutyCycle0);
    myPrint(highTime1, lowTime1, dutyCycle1);

    float dutyCycleChange0 = dutyCycle0-lastDutyCycle0;

    float a = 3;
    float k = 2.5/(pow(a,2.5)-1);
    if (dutyCycleChange0 > 0){
      dutyCycleChange0 = k * (pow(a,dutyCycleChange0)-1);
    } else{
      dutyCycleChange0 = -k * (pow(a,-dutyCycleChange0)-1);
    }

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

    Serial.print(finalDutyCycle0);
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

    myLedcWrite(THROTTLE_OUTPUT,(uint32_t)(finalDutyCycle0/100.0*65535.0));

    lastDutyCycle0 = finalDutyCycle0;

    delay(20 / portTICK_PERIOD_MS);
  }
}

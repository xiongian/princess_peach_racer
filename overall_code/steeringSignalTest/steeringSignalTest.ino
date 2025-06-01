#define PWM_PIN0 34
#define OUTPUT_PIN 5

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(PWM_PIN0, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long highTime0 = pulseIn(PWM_PIN0, HIGH);
  // Measure LOW duration
  unsigned long lowTime0 = pulseIn(PWM_PIN0, LOW);

  // Calculate period and duty cycle
  unsigned long period0 = highTime0 + lowTime0;
  float dutyCycle0 = (highTime0 * 100.0) / period0;

  Serial.print("High Time 0: ");
  Serial.print(highTime0);
  Serial.print(" us, Low Time 0: ");
  Serial.print(lowTime0);
  Serial.print(" us, Duty Cycle 0: ");
  Serial.print(dutyCycle0);
  Serial.println(" %");

}
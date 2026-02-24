#define SENSOR_PIN A0
#define NUM_SAMPLES 10
#define BASELINE_SAMPLES 600

const int ledRed = 9;
const int ledGreen = 10; // optional
const int ledBlue = 11;

float readings[NUM_SAMPLES];
int index = 0;
float baselineVoltage = 0;
bool baselineSet = false;

void setup() {
  Serial.begin(9600);
  pinMode(ledRed, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(ledBlue, OUTPUT);

  // Initialize array
  for (int i = 0; i < NUM_SAMPLES; i++) readings[i] = 0;

  // Take baseline readings
  Serial.println("Calibrating baseline, please leave container empty...");

  float sum = 0;
  for (int i = 0; i < BASELINE_SAMPLES; i++) {
    float v = analogRead(SENSOR_PIN) * (3.3 / 1023.0);
    sum += v;
    delay(200); // 200ms delay between readings
  }
  baselineVoltage = sum / BASELINE_SAMPLES;
  baselineSet = true;

  Serial.print("Baseline set to: ");
  Serial.println(baselineVoltage, 3);
}

void loop() {
  if (!baselineSet) return;

  // Smooth readings with rolling average
  readings[index] = analogRead(SENSOR_PIN) * (3.3 / 1023.0);
  index = (index + 1) % NUM_SAMPLES;

  float avg = 0;
  for (int i = 0; i < NUM_SAMPLES; i++) avg += readings[i];
  avg /= NUM_SAMPLES;

  Serial.print("Sensor Voltage: ");
  Serial.print(avg, 3);
  Serial.print(" | ΔV: ");
  float delta = avg - baselineVoltage;
  if (delta < 0) delta = 0; //
  Serial.println(delta, 3);

  // LED fade range: 0.03V to 0.15V above baseline
  float minDelta = 0.004;
  float maxDelta = 0.020;

  // Clamp delta within range
  delta = constrain(delta, minDelta, maxDelta);

  // Map delta to fade range
  int fade = map((int)(delta * 1000), (int)(minDelta * 1000), (int)(maxDelta * 1000), 0, 255);

  // Set LED colors
  analogWrite(ledRed, fade);
  analogWrite(ledBlue, 255 - fade);
  analogWrite(ledGreen, 0); // optional

  delay(1000);
}

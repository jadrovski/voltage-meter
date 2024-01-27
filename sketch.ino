#define MODE_RAW 0
#define MODE_CAL 1
#define MODE_VOL 2

#define CURRENT_MODE MODE_VOL

#define ADC_PIN A0
#define ADC_MAX 592
#define ADC_MIN 453
#define VOLT_MULTI 232

void setup() {
  
  Serial.begin(115200);
}

void loop() {
  switch (CURRENT_MODE) {
    case MODE_RAW:
      modeRaw();
      break;
    case MODE_CAL:
      modeCalibrate();
      break;
    case MODE_VOL:
      modePrintVoltage();
      break;
    default:
      Serial.println("Wrong mode");
      break;
  }
}

void modeRaw() {
  Serial.println(analogRead(ADC_PIN));
  delay(100);
}

void modeCalibrate() {
  int adc_max = 0;             // Variable to store the maximum sensor value
  int adc_min = 1023;          // Variable to store the minimum sensor value
  long tiempo_init = millis(); // Get the current time in milliseconds
  
  while (true) {
    if ((millis() - tiempo_init) > 500) { // Check if 500 milliseconds have passed
      adc_max = 0;         // Reset the maximum sensor value
      adc_min = 1023;      // Reset the minimum sensor value
      tiempo_init = millis(); // Update the initial time
    }
   
    int sensorValue = analogRead(ADC_PIN); // Read the analog input from pin ADC_PIN
   
    if (sensorValue > adc_max) {
      adc_max = sensorValue; // Update the maximum value if a new maximum is found
    } else if (sensorValue < adc_min) {
      adc_min = sensorValue; // Update the minimum value if a new minimum is found
    }
   
    // Print the maximum and minimum values to the serial monitor
    Serial.print("adc_max: ");
    Serial.print(adc_max);
    Serial.print("   adc_min: ");
    Serial.println(adc_min);
   
    delay(1); // Small delay for stability between each iteration
  }
}

float volt_multi_p;
float volt_multi_n;

void modePrintVoltage() {
  volt_multi_p = VOLT_MULTI * 1.4142;   // Peak voltage = RMS voltage * 1.4142 (Single-phase current)
  volt_multi_n = -volt_multi_p;         // Negative peak voltage
  
  while (true) {
    float volt_rms = getVoltage();       // Root Mean Square voltage (V-RMS)
    Serial.print("Vrms: ");
    Serial.print(volt_rms, 3);
    Serial.println(" VAC");
    // Delay for a certain interval if needed
    //delay(100);
  }
}

float getVoltage() {
  float adc_sample;
  float volt_inst = 0;
  float sum = 0;
  float volt;
  long init_time = millis();
  int N = 0;

  while ((millis() - init_time) < 500) { // Duration of 0.5 seconds (Approximately 30 cycles of 60Hz)
    adc_sample = analogRead(ADC_PIN); // Sensor voltage
    volt_inst = map(adc_sample, ADC_MIN, ADC_MAX, volt_multi_n, volt_multi_p);
    sum += sq(volt_inst); // Sum of Squares
    N++;
    delay(1);
  }

  //Serial.print("N: ");
  //Serial.println(N);

  volt = sqrt(sum / N); // RMS equation
  return volt;
}

// Pin assignments
const int relayPin = 3;
const int redLedPin = 5;
const int greenLedPin = 6;
const int lowVoltageLedPin = 4;

// Voltage monitor pins
const int shutdownVoltagePin = A0;
const int lowVoltagePin = A1;

// Shutdown thresholds (adjust as needed)
const float shutdownThreshold = 2.0;  // volts (scaled via voltage divider)
const float lowVoltageThreshold = 2.0; // volts (scaled via voltage divider)

// Timing
const int shortPressTime = 1000; // ms
const int longPressTime = 5000; // ms
const int lowVoltageFlashInterval = 500; // ms

// Variables
bool os_ready = false;
unsigned long previousFlashMillis = 0;
bool lowVoltageLedState = false;

void setup() {
  pinMode(relayPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(lowVoltageLedPin, OUTPUT);

  digitalWrite(relayPin, LOW);
  digitalWrite(redLedPin, HIGH);  // Default to red ON
  digitalWrite(greenLedPin, LOW); // Green OFF
  digitalWrite(lowVoltageLedPin, LOW);

  Serial.begin(9600);
}

void loop() {
  // Read shutdown voltage
  float shutdownVoltage = analogRead(shutdownVoltagePin) * (5.0 / 1023.0);
  if (shutdownVoltage < shutdownThreshold) {
    triggerShutdown();
  }

  // Read low voltage
  float lowVoltage = analogRead(lowVoltagePin) * (5.0 / 1023.0);
  if (lowVoltage < lowVoltageThreshold) {
    flashLowVoltageLed();
  } else {
    digitalWrite(lowVoltageLedPin, LOW);
  }

  // Check for serial commands
  if (Serial.available() > 0) {
    char incomingByte = Serial.read();
    if (incomingByte == '1') {
      os_ready = true;
      updateStatusLeds();
    } else if (incomingByte == '0') {
      os_ready = false;
      updateStatusLeds();
    }
  }
}

void triggerShutdown() {
  // Determine if short or long press
  int pressDuration = os_ready ? shortPressTime : longPressTime;

  // Pulse relay
  digitalWrite(relayPin, HIGH);
  delay(pressDuration);
  digitalWrite(relayPin, LOW);

  // If short press was done, clear OS Ready flag
  if (os_ready) {
    os_ready = false;
    updateStatusLeds();
  }

  // Small debounce delay
  delay(1000);
}

void flashLowVoltageLed() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousFlashMillis >= lowVoltageFlashInterval) {
    previousFlashMillis = currentMillis;
    lowVoltageLedState = !lowVoltageLedState;
    digitalWrite(lowVoltageLedPin, lowVoltageLedState ? HIGH : LOW);
  }
}

void updateStatusLeds() {
  if (os_ready) {
    digitalWrite(redLedPin, LOW);
    digitalWrite(greenLedPin, HIGH);
  } else {
    digitalWrite(redLedPin, HIGH);
    digitalWrite(greenLedPin, LOW);
  }
}

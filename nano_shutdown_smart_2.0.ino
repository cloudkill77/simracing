// Pin definitions
#define RELAY_PIN 3       // Relay control pin (connected to transistor/MOSFET)
#define LOW_VOLT_LED 4    // LED for low voltage warning (flashing)
#define RED_LED 5         // LED ON when OS_READY == false
#define GREEN_LED 6       // LED ON when OS_READY == true

// Timing
const unsigned long SHORT_PRESS_DURATION = 1000;   // 1 second pulse for graceful shutdown
const unsigned long LONG_PRESS_DURATION  = 5000;   // 5 seconds pulse for forced shutdown

// Analog pins
#define SHUTDOWN_VOLTAGE_PIN A0
#define LOW_VOLTAGE_PIN A1

// Thresholds
#define SHUTDOWN_TRIGGER_THRESHOLD 500   // Adjust per your voltage divider (shutdown detect)
#define LOW_VOLTAGE_THRESHOLD 500        // Adjust per your voltage divider (low battery detect)

// Variables
bool os_ready = false;
unsigned long lowVoltLastToggle = 0;
bool lowVoltLedState = false;
const unsigned long lowVoltBlinkInterval = 500; // 500 ms blink interval

void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  pinMode(LOW_VOLT_LED, OUTPUT);
  digitalWrite(LOW_VOLT_LED, LOW);

  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  updateStatusLEDs();

  Serial.begin(115200);
  os_ready = false;
  updateStatusLEDs();
}

void updateStatusLEDs() {
  if (os_ready) {
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
  } else {
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);
  }
}

void shutdownSequence() {
  if (os_ready) {
    // Graceful shutdown: short relay pulse
    digitalWrite(RELAY_PIN, HIGH);
    delay(SHORT_PRESS_DURATION);
    digitalWrite(RELAY_PIN, LOW);

    // Clear os_ready flag after shutdown command
    os_ready = false;
    updateStatusLEDs();

    Serial.println("Graceful shutdown triggered (short press)");
  } else {
    // Forced shutdown: long relay pulse
    digitalWrite(RELAY_PIN, HIGH);
    delay(LONG_PRESS_DURATION);
    digitalWrite(RELAY_PIN, LOW);

    Serial.println("Forced shutdown triggered (long press)");
  }
}

void loop() {
  // Serial input for OS_READY or SHUTDOWN commands
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd.equalsIgnoreCase("READY")) {
      os_ready = true;
      Serial.println("OS_READY set");
      updateStatusLEDs();
    } else if (cmd.equalsIgnoreCase("NOTREADY")) {
      os_ready = false;
      Serial.println("OS_READY cleared");
      updateStatusLEDs();
    } else if (cmd.equalsIgnoreCase("SHUTDOWN")) {
      Serial.println("Shutdown requested via serial");
      shutdownSequence();
    }
  }

  // Check for shutdown trigger on A0
  int shutdownVoltage = analogRead(SHUTDOWN_VOLTAGE_PIN);
  if (shutdownVoltage < SHUTDOWN_TRIGGER_THRESHOLD) {
    Serial.println("Shutdown voltage detected, triggering shutdown");
    shutdownSequence();
    delay(5000); // Debounce / avoid repeated shutdown triggers
  }

  // Low voltage check on A1 (flashing LED)
  int voltageReading = analogRead(LOW_VOLTAGE_PIN);
  if (voltageReading < LOW_VOLTAGE_THRESHOLD) {
    unsigned long now = millis();
    if (now - lowVoltLastToggle > lowVoltBlinkInterval) {
      lowVoltLedState = !lowVoltLedState;
      digitalWrite(LOW_VOLT_LED, lowVoltLedState ? HIGH : LOW);
      lowVoltLastToggle = now;
    }
  } else {
    // Voltage okay, LED off
    digitalWrite(LOW_VOLT_LED, LOW);
    lowVoltLedState = false;
  }
}

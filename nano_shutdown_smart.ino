const int relayPin = 3;    // Relay control pin
const int analogPin = A0;  // Voltage monitoring pin

bool osReady = false;

void setup() {
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);

  Serial.begin(9600);

  // Optional: Small delay for serial startup
  delay(1000);
}

void loop() {
  // Check for serial data from the PC
  if (Serial.available()) {
    String msg = Serial.readStringUntil('\n');
    msg.trim();

    if (msg == "OS_READY") {
      osReady = true;
      Serial.println("ACK");
    }
  }

  // Read voltage at A0
  int voltageReading = analogRead(analogPin);

  // Adjust threshold for your actual voltage divider setup
  if (voltageReading < 100) {  // Example threshold
    shutdownPC();
    delay(10000);  // Debounce/avoid retriggering
  }

  // Do nothing otherwise
}

void shutdownPC() {
  if (osReady) {
    // Short press (1 sec) for graceful shutdown
    digitalWrite(relayPin, HIGH);
    delay(1000);
    digitalWrite(relayPin, LOW);
  } else {
    // Long press (6 sec) for forced shutdown
    digitalWrite(relayPin, HIGH);
    delay(6000);
    digitalWrite(relayPin, LOW);
  }
}

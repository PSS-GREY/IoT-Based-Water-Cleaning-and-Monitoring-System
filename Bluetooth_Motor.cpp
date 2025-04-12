#include <SoftwareSerial.h>

#define MOTOR_PIN 8
#define PUMP_PIN 7

SoftwareSerial bluetooth(3, 4); // RX, TX

String command = "";

void setup() {
  Serial.begin(9600);         // Serial to ESP32
  bluetooth.begin(9600);      // Bluetooth module
  pinMode(MOTOR_PIN, OUTPUT);
  pinMode(PUMP_PIN, OUTPUT);

  digitalWrite(MOTOR_PIN, HIGH); // active-low relay OFF
  digitalWrite(PUMP_PIN, HIGH);

  Serial.println("Arduino Nano Ready");
}

void loop() {
  // From ESP32
  if (Serial.available()) {
    command = Serial.readStringUntil('\n');
    command.trim();
    Serial.println("From ESP: " + command);
    if (command.equalsIgnoreCase("CLEAN")) {
      startCleaning();
    }
  }

  // From Bluetooth
  if (bluetooth.available()) {
    command = bluetooth.readStringUntil('\n');
    command.trim();
    Serial.println("From BT: " + command);
    if (command.equalsIgnoreCase("turn on")) {
      startCleaning();
    }
  }
}

void startCleaning() {
  Serial.println("Cleaning started...");

  digitalWrite(MOTOR_PIN, LOW);  // Turn ON motor (active-low)
  delay(10000);
  digitalWrite(MOTOR_PIN, HIGH); // Turn OFF motor

  digitalWrite(PUMP_PIN, LOW);   // Turn ON pump
  delay(10000);
  digitalWrite(PUMP_PIN, HIGH);  // Turn OFF pump

  Serial.println("Cleaning completed.");
}

#define BLYNK_TEMPLATE_ID "TMPL3NY1dAUuq"
#define BLYNK_TEMPLATE_NAME "Water"
#define BLYNK_PRINT Serial

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// ---------- Pin Definitions ----------
#define TRIG_PIN 5
#define ECHO_PIN 18
#define TURBIDITY_PIN 34
#define TDS_SENSOR_PIN 35
#define TANK_HEIGHT 20.0  // in cm

// ---------- Blynk Auth and WiFi ----------
char auth[] = "axvJ5AhNlqv3JrtoNam5CZzdp7TcaIsO";
char ssid[] = "Techno Spark Air 6";
char pass[] = "sunday13";

// ---------- LCD and Serial Setup ----------
LiquidCrystal_I2C lcd(0x27, 16, 2);
WidgetLCD blynkLcd(V6);
HardwareSerial ArduinoSerial(2); // RX = 16, TX = 17

BlynkTimer timer;
int displayState = 0;
bool cleanSent = false;

// ---------- Function Prototypes ----------
void cycleDisplay();
float getTDS();
float getTurbidity();
int getWaterLevel();
float mapTurbidity(float voltage);
void sendCleanCommand();

void setup() {
  Serial.begin(115200);
  ArduinoSerial.begin(9600, SERIAL_8N1, 16, 17);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(TURBIDITY_PIN, INPUT);
  pinMode(TDS_SENSOR_PIN, INPUT);

  lcd.init();
  lcd.backlight();

  Blynk.begin(auth, ssid, pass);

  timer.setInterval(3000L, cycleDisplay);
  Serial.println("System Initialized...");
}

void loop() {
  Blynk.run();
  timer.run();
}

// ---------- Water Level ----------
int getWaterLevel() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  float distance = duration * 0.034 / 2.0;
  int level = TANK_HEIGHT - distance;
  level = constrain(level, 0, TANK_HEIGHT);
  int percent = round((level / TANK_HEIGHT) * 100);

  Blynk.virtualWrite(V0, percent);
  Serial.print("Water Level: ");
  Serial.print(percent);
  Serial.println("%");

  return percent;
}

// ---------- Turbidity ----------
float getTurbidity() {
  int value = analogRead(TURBIDITY_PIN);
  float voltage = value * (5.0 / 4095.0);

  float turbidity = mapTurbidity(voltage);
  Blynk.virtualWrite(V1, turbidity);
  Serial.print("Turbidity: ");
  Serial.print(turbidity);
  Serial.println(" NTU");

  return turbidity;
}

float mapTurbidity(float voltage) {
  float voltRange[] = {1.95, 1.5, 1.4, 1.3, 1.2, 1.1, 1.0, 0.9, 0.8, 0.7, 0.6, 0.0};
  float ntuRange[]  = {0, 2.5, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50};

  if (voltage >= voltRange[0]) return ntuRange[0];
  if (voltage <= voltRange[11]) return ntuRange[11];

  for (int i = 0; i < 11; i++) {
    if (voltage < voltRange[i] && voltage >= voltRange[i + 1]) {
      float m = (ntuRange[i + 1] - ntuRange[i]) / (voltRange[i + 1] - voltRange[i]);
      return ntuRange[i] + m * (voltage - voltRange[i]);
    }
  }
  return 0;
}

// ---------- TDS ----------
float getTDS() {
  int value = analogRead(TDS_SENSOR_PIN);
  float voltage = value * (5.0 / 1024.0);

  float ec = (133.42 * pow(voltage, 3) - 255.86 * pow(voltage, 2) + 857.39 * voltage) * 0.5;
  float tds = ec * 0.5;

  Blynk.virtualWrite(V5, tds);
  Serial.print("TDS: ");
  Serial.print(tds);
  Serial.println(" ppm");

  return tds;
}

// ---------- Send CLEAN Command ----------
void sendCleanCommand() {
  Serial.println("Sending CLEAN command to Arduino...");
  ArduinoSerial.println("CLEAN");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Cleaning Started");

  blynkLcd.clear();
  blynkLcd.print(0, 0, "Cleaning Started");

  Blynk.logEvent("cleaning_started", "Cleaning has been triggered!");
}

// ---------- Display Function ----------
void cycleDisplay() {
  lcd.clear();
  blynkLcd.clear();

  switch (displayState) {
    case 0: {
      int waterLevel = getWaterLevel();

      lcd.setCursor(0, 0);
      lcd.print("Water Level:");
      lcd.setCursor(0, 1);
      lcd.print(waterLevel);
      lcd.print("%");

      blynkLcd.print(0, 0, "Water Level");
      blynkLcd.print(0, 1, String(waterLevel) + "%");
      break;
    }

    case 1: {
      float tds = getTDS();

      lcd.setCursor(0, 0);
      lcd.print("TDS:");
      lcd.setCursor(0, 1);
      lcd.print(tds, 0);
      lcd.print(" ppm");

      blynkLcd.print(0, 0, "TDS Reading");
      blynkLcd.print(0, 1, String(tds, 0) + " ppm");
      break;
    }

    case 2: {
      float turbidity = getTurbidity();

      lcd.setCursor(0, 0);
      lcd.print("Turbidity:");
      lcd.setCursor(0, 1);
      lcd.print(turbidity, 1);
      lcd.print(" NTU");

      blynkLcd.print(0, 0, "Turbidity");
      blynkLcd.print(0, 1, String(turbidity, 1) + " NTU");

      int waterLevel = getWaterLevel();
      float tds = getTDS();
      bool qualityBad = (turbidity > 5.0 || tds > 500.0);

      if (qualityBad && waterLevel <= 20) {
        if (!cleanSent) {
          sendCleanCommand();
          cleanSent = true;
        }
      } else if (qualityBad && waterLevel > 20) {
        if (!cleanSent) {
          Serial.println("Sending ALERT to Arduino...");
          ArduinoSerial.println("ALERT");

          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Drain Required");

          blynkLcd.clear();
          blynkLcd.print(0, 0, "Alert: Drain Needed");
          Blynk.logEvent("drain_required", "Water quality poor, drain water.");
          cleanSent = true;
        }
      } else {
        cleanSent = false;
      }
      break;
    }
  }

  displayState = (displayState + 1) % 3;
}

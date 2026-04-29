#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <max6675.h>

// LCD setup (I2C address 0x27; change if needed)
LiquidCrystal_I2C lcd(0x27, 20, 4);

// MAX6675 pins
const int thermoSO = 12;
const int thermoCS = 11;
const int thermoSCK = 10;

MAX6675 thermocouple(thermoSCK, thermoCS, thermoSO);

// Potentiometer and Relay
const int potPin = A0;
const int relayPin = 8;

// Control settings
const float maxTempSet = 300.0; // Max setpoint in °C
const float hysteresis = 1.0;   // °C buffer to avoid relay chatter

bool heaterOn = false; // Relay state

// Function: Get average temperature
float getAverageTemperature(int samples = 10) {
  float total = 0;
  for (int i = 0; i < samples; i++) {
    total += thermocouple.readCelsius();
    delay(50);
  }
  return total / samples;
}

void setup() {
  Serial.begin(9600);

  // Relay output
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH); // Heater OFF initially

  // LCD setup
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Temp Control Ready");

  delay(2000);
  lcd.clear();
}

void loop() {
  // Read and map potentiometer value to temperature setpoint (0 - 300°C)
  int potValue = analogRead(potPin);
  float setTemp = map(potValue, 0, 1023, 0, (int)maxTempSet);

  // Read actual temperature (averaged)
  float currentTemp = getAverageTemperature(10);

  // Temperature control with hysteresis
  if (!heaterOn && currentTemp < (setTemp - hysteresis)) {
    digitalWrite(relayPin, HIGH); // Heater ON
    heaterOn = true;
  } else if (heaterOn && currentTemp >= setTemp) {
    digitalWrite(relayPin, LOW); // Heater OFF
    heaterOn = false;
  }

  // Serial monitor output
  Serial.print("Set Temp: ");
  Serial.print(setTemp);
  Serial.print(" C | Current Temp: ");
  Serial.print(currentTemp);
  Serial.print(" C | Heater: ");
  Serial.println(heaterOn ? "ON" : "OFF");

  // LCD output
  lcd.setCursor(0, 0);
  lcd.print("Set: ");
  lcd.print(setTemp, 1);
  lcd.print(" C      ");

  lcd.setCursor(0, 1);
  lcd.print("Temp: ");
  lcd.print(currentTemp, 1);
  lcd.print(" C     ");

  lcd.setCursor(0, 2);
  lcd.print("Heater: ");
  lcd.print(heaterOn ? "ON " : "OFF");

  delay(500); // Update interval
}

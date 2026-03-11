#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>


#define SOIL_PIN 34
#define DHT_PIN 4
#define DHT_TYPE DHT11

#define RED_PIN 25
#define GREEN_PIN 26
#define BLUE_PIN 27


#define SOIL_DRY_THRESHOLD 3000
#define SOIL_MEDIUM_THRESHOLD 1500


int temperature = 0.0, humidity = 0.0;
int soilMoistureValue;
int soilPercent;

DHT dht(DHT_PIN, DHT_TYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);


void setup() {
  Serial.begin(115200);

  dht.begin();
  lcd.init();
  lcd.backlight();

  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
}


void loop() {
  readSensors();
  printToSerial();
  updateLCD();
  updateRGB();
  delay(2000);
}

// --- Read Sensor Values ---
void readSensors() {
  soilMoistureValue = analogRead(SOIL_PIN);
  soilPercent = map(soilMoistureValue, 4095, 0, 0, 100);

  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    humidity = 0;
    temperature = 0;
  }
}

// --- Serial Output ---
void printToSerial() {
  Serial.print("Soil Moisture: ");
  Serial.print(soilMoistureValue);
  Serial.print(" (");
  Serial.print(soilPercent);
  Serial.print("%)\t");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print("%\tTemperature: ");
  Serial.print(temperature);
  Serial.println("°C");
}

// --- LCD Display ---
void updateLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Soil : ");
  lcd.print(soilMoistureValue);
  lcd.print(" (");
  lcd.print(soilPercent);
  lcd.print("%)");

  lcd.setCursor(0, 1);
  lcd.print("Condition : ");
  if (soilMoistureValue > SOIL_DRY_THRESHOLD) {
    lcd.print("Dry");
  } else if (soilMoistureValue > SOIL_MEDIUM_THRESHOLD) {
    lcd.print("Med.");
  } else {
    lcd.print("Wet");
  }

  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp. : ");
  lcd.print(" C");
  lcd.print(temperature);
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print("Humi. : ");
  lcd.print(humidity);
  lcd.print("%");
}

// --- RGB LED Control ---
void updateRGB() {
  if (soilMoistureValue > SOIL_DRY_THRESHOLD) {
    setRGB(1, 0, 0);  // Red = Dry
  } else if (soilMoistureValue > SOIL_MEDIUM_THRESHOLD) {
    setRGB(0, 0, 1);  // Blue = Medium
  } else {
    setRGB(0, 1, 0);  // Green = Wet
  }
}

// --- Helper to Set RGB Pins ---
void setRGB(bool r, bool g, bool b) {
  digitalWrite(RED_PIN, r);
  digitalWrite(GREEN_PIN, g);
  digitalWrite(BLUE_PIN, b);
}

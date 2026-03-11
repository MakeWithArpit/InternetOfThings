#include <ACS712.h>

ACS712 sensor1(34, 3.3, 4095); // GPIO34 pin, 3.3V ESP32, 12-bit ADC
ACS712 sensor2(35, 3.3, 4095); // GPIO35 pin, 3.3V ESP32, 12-bit ADC

void setup() {
  Serial.begin(115200);

  Serial.println("Calibrating sensors...");
  delay(2000); // sensor ko stable hone do

  sensor1.autoMidPoint(); // automatic calibration
  sensor2.autoMidPoint();

  Serial.println("Calibration Done");
}

void loop() {
  float current1 = sensor1.mA_AC(50) / 1000.0; // 50 samples leke average current
  float current2 = sensor2.mA_AC(50) / 1000.0;

  Serial.print("Sensor 1 Current: ");
  Serial.print(current1, 2);
  Serial.println(" A");

  Serial.print("Sensor 2 Current: ");
  Serial.print(current2, 2);
  Serial.println(" A");

  delay(1000); // 1 second delay
}

#include <Wire.h>
#include <RTClib.h>

RTC_DS3231 rtc;

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);  // SDA, SCL for ESP32

  if (!rtc.begin()) {
    Serial.println("RTC not found");
    while (1);
  }

  //rtc.adjust(DateTime(2025, 5, 1, 22, 17, 10)); // Set manually
}

void loop() {
  DateTime now = rtc.now();
  Serial.println(rtc.now().unixtime());

  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.println(now.second(), DEC);

  delay(1000);
}

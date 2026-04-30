/*
 * LED Blink — OTA se aayega yeh code
 * Version: 1.0
 */

#include <WiFi.h>
#include <Preferences.h>

#define LED_PIN 2

Preferences preferences;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  Serial.println("LED Blink v1.0 — OTA se aaya!");
}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  delay(500);
  digitalWrite(LED_PIN, LOW);
  delay(500);
}
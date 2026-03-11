#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "YOUR HOTSPOT NAME";
const char* password = "YOUR HOTSPOT PASSWORD";
const char* serverName = "https://script.google.com/macros/s/............/";  // <<--- YOUR SCRIPT URL

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {

    StaticJsonDocument<256> jsonDoc;
    jsonDoc["data1"] = 123.45;
    jsonDoc["data2"] = 67.89;
    jsonDoc["data3"] = "Normal";

    String jsonStr;
    serializeJson(jsonDoc, jsonStr);

    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "application/json");

    int postCode = http.POST(jsonStr);
    if (postCode > 0) {
      Serial.println("POST Success: " + String(postCode));
    } else {
      Serial.println("POST Failed, code: " + String(postCode));
    }

    http.end();
  }
  delay(1000);
}

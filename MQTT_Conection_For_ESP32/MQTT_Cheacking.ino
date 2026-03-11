#include <WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>


const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* mqttServer = "demo.thingsboard.io";
const int mqttPort = 1883;
const char* token = "BIgOOSVqrCcujHUENOWh";

float temperature;

WiFiClient espClient;
PubSubClient client(espClient);


void ConnectToNetwork() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 16) {
    delay(1000);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to WiFi.");
  } else {
    Serial.println("\nFailed to connect to WiFi.");
  }
}

void ConnectToServer() {
  while (!client.connected()) {
    Serial.print("Connecting to ThingsBoard...");
    if (client.connect("ESP32_Client", token, "")) {
      Serial.println("Connected to ThingsBoard");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void DataSending() {
  StaticJsonDocument<200> doc;
  doc["Temperature"] = temperature;
  char buffer[256];
  serializeJson(doc, buffer);
  client.publish("v1/devices/me/telemetry", buffer);
}

void setup() {

  Serial.begin(115200);
  ConnectToNetwork();
  client.setServer(mqttServer, mqttPort);
  ConnectToServer();
}

void loop() {
  if (!client.connected()) {
    ConnectToServer();
  }
  client.loop();

  temperature = random(0.0,150.0);


  DataSending();
  delay(2000);
}

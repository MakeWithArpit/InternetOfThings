#include <PubSubClient.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_SGP30.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

Adafruit_SGP30 sgp;
Adafruit_SSD1306 display(128, 32, &Wire);
DHT dht(5, DHT11);

#define SCREEN_WIDTH 128;
#define SCREEN_HIGHT 32;
#define OLED_RESET -1;
#define red 13
#define blue 12
#define green 11

float temp;
float hum;
int tvoc;
int eco2;
const char* ssid = "OnePlus";
const char* pass = "12345678iop";
const char* mqtt_server = "demo.thingsboard.io";
const char* token = "m9qcjS2H1PQ52aFCbSVL";

WiFiClient espClient;
PubSubClient client(espClient);

void connectToNetwork() {
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println(".");
    delay(400);
  }
  Serial.println("Connected to Network !");
  Serial.println("Network macAddress :- ");
  Serial.print(WiFi.macAddress());
}

void connectToServer() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32_DHT11", token, NULL)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(2000);
    }
  }
}

void DHT11sen() {
  temp = dht.readTemperature();
  hum = dht.readHumidity();
  if (isnan(temp) || isnan(hum)) {
    Serial.println("DHT Sensor Failure");
  }
}

void SGP30sen() {
  tvoc = sgp.TVOC;
  eco2 = sgp.eCO2;
}

void DataMoniter() {
  // DISPLAY DATA ON SEREAL MONITER AND OLED
  display.clearDisplay();

  Serial.print("Temperature : ");
  Serial.print(temp);
  Serial.println(" *C");
  delay(400);
  display.setCursor(0, 0);
  display.print("Temp : ");
  display.print(temp);
  display.print("*C");
  delay(400);

  Serial.print("Humidity :");
  Serial.print(hum);
  Serial.println(" %");
  delay(400);
  display.setCursor(1, 0);
  display.print("Hum : ");
  display.print(hum);
  display.print("%");
  delay(400);

  Serial.print("TVOC :");
  Serial.print(tvoc);
  Serial.println(" ppb");
  delay(400);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("TVOC : ");
  display.print(tvoc);
  display.print("ppb");
  delay(400);

  Serial.print("ECO2 : ");
  Serial.print(eco2);
  Serial.println(" ppm");
  delay(400);
  display.setCursor(1, 0);
  display.print("ECO2 : ");
  display.print(eco2);
  display.print("ppm");
  delay(400);
}

void rgb() {
  if (eco2 >= 400 && eco2 <= 1000 && tvoc >= 0 && tvoc <= 250)
    digitalWrite(green, 1);

  if (eco2 >= 1001 && eco2 <= 1500 && tvoc >= 251 && tvoc <= 650)
    digitalWrite(blue, 1);

  if (eco2 >= 1501 && eco2 <= 2100 && tvoc >= 651 && tvoc <= 5000)
    digitalWrite(red, 1);
}

void DataSending() {
  // DATA SEND TO THE SERVER
  if (!client.connected()) {
    connectToServer();
  }
  client.loop();

  String payload = "{\"temperature\":";
  payload += temp;
  payload += ",\"humidity\":";
  payload += hum;
  payload += ",\"TVOC\":";
  payload += tvoc;
  payload += ",\"ECO2\":";
  payload += eco2;
  payload += "}";

  Serial.println("Sending payload: ");
  Serial.print(payload);
  client.publish("v1/devices/me/telemetry", payload.c_str());
}

void setup() {
  Serial.begin(115200);
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  Wire.begin();
  client.setServer(mqtt_server, 1883);
  sgp.begin();
  dht.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  connectToNetwork();
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
}

void loop() {
  DHT11sen();
  SGP30sen();
  rgb();
  DataMoniter();
  delay(2000);
}
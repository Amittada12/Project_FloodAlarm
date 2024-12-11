#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Arduino_JSON.h>
#include <FS.h>
#include <LittleFS.h>

// WiFi credentials
const char* ssid = "ume";
const char* password = "123";

// Sensor Pins
const int waterLevel1 = A0;
const int waterLevel2 = A1;
const int waterLevel3 = A2;
const int flowRatePin = D2;

// Flow Rate Variables
volatile int pulseCount = 0;
float flowRate = 0;
unsigned long previousMillis = 0;

// Web Server
AsyncWebServer server(80);

// JSON variable to hold sensor readings
JSONVar sensorReadings;

// Variables for alert and notifications
bool enableNotifications = true;
float alertLevel1 = 1.30; // Meter
float alertLevel2 = 1.50; // Meter
float alertLevel3 = 1.80; // Meter

// ISR for Flow Rate Sensor
void pulseCounter() {
  pulseCount++;
}

// Get Water Level Readings
float getWaterLevel(int pin) {
  return analogRead(pin) * (3.3 / 1024.0); // Convert to Voltage
}

// Get Flow Rate
float getFlowRate() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > 1000) {
    flowRate = (pulseCount / 7.5); // Calculate flow rate in L/min
    pulseCount = 0;
    previousMillis = currentMillis;
  }
  return flowRate;
}

// Prepare JSON Data
String getSensorData() {
  sensorReadings["waterLevel1"] = getWaterLevel(waterLevel1);
  sensorReadings["waterLevel2"] = getWaterLevel(waterLevel2);
  sensorReadings["waterLevel3"] = getWaterLevel(waterLevel3);
  sensorReadings["flowRate"] = getFlowRate();
  return JSON.stringify(sensorReadings);
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Initialize Pins
  pinMode(flowRatePin, INPUT);
  attachInterrupt(digitalPinToInterrupt(flowRatePin), pulseCounter, RISING);

  // Initialize LittleFS
  if (!LittleFS.begin()) {
    Serial.println("An error occurred while mounting LittleFS");
    return;
  }

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected! IP: " + WiFi.localIP().toString());

  // Serve static files
  server.serveStatic("/", LittleFS, "/index.html");
  server.serveStatic("/styles.css", LittleFS, "/styles.css");
  server.serveStatic("/script.js", LittleFS, "/script.js");

  // API Endpoint to get sensor data
  server.on("/api/sensors", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "application/json", getSensorData());
  });

  // API Endpoint to toggle notifications
  server.on("/api/toggle-notifications", HTTP_POST, [](AsyncWebServerRequest* request) {
    enableNotifications = !enableNotifications;
    request->send(200, "application/json", "{\"notifications\": " + String(enableNotifications) + "}");
  });

  // Start server
  server.begin();
}

void loop() {
  // Monitor Water Levels and Alert
  float level1 = getWaterLevel(waterLevel1);
  float level2 = getWaterLevel(waterLevel2);
  float level3 = getWaterLevel(waterLevel3);

  if (enableNotifications) {
    if (level3 >= alertLevel3) {
      Serial.println("Red Alert: Extreme Danger!");
    } else if (level2 >= alertLevel2) {
      Serial.println("Orange Alert: High Risk!");
    } else if (level1 >= alertLevel1) {
      Serial.println("Yellow Alert: Moderate Risk!");
    }
  }
}

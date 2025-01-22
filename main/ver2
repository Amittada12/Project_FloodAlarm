#include <ESP8266WiFi.h>

// WiFi Credentials
const char* ssid = ">3";        // ชื่อ WiFi
const char* password = "12345678"; // รหัสผ่าน WiFi

// Float Switch Pins
const int waterLevelLow = 5;   // ระดับต่ำสุด - สีเขียว
const int waterLevelMid = 4;   // ระดับปานกลาง - สีเหลือง
const int waterLevelHigh = 0;  // ระดับสูงสุด - สีแดง

// Flow Rate Sensor Pin
const int flowRatePin = 2;

// Variables for Flow Rate Sensor
volatile int pulseCount = 0;
float flowRate = 0;
unsigned long previousMillis = 0;

// Web Server on Port 80
WiFiServer server(80);

// ISR for Flow Rate Sensor
void pulseCounter() {
  pulseCount++;
}

// Get Flow Rate
float getFlowRate() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > 10000) {
    flowRate = (pulseCount / 7.5); // Calculate flow rate in L/min
    pulseCount = 0;
    previousMillis = currentMillis;
  }
  return flowRate;
}

// Get Water Level State
bool getWaterLevel(int pin) {
  return digitalRead(pin) == LOW;
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  delay(10);

  // Initialize Pins
  pinMode(waterLevelLow, INPUT_PULLUP);
  pinMode(waterLevelMid, INPUT_PULLUP);
  pinMode(waterLevelHigh, INPUT_PULLUP);
  pinMode(flowRatePin, INPUT);

  // Attach Interrupt for Flow Rate Sensor
  attachInterrupt(digitalPinToInterrupt(flowRatePin), pulseCounter, RISING);

  // Connect to WiFi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Start Web Server
  server.begin();
  Serial.println("Web server started");
}

void loop() {
  WiFiClient client = server.available(); // Check if a client has connected
  if (client) {
    Serial.println("New Client connected");
    String request = client.readStringUntil('\r');
    Serial.println(request);

    // Prepare HTML Response
    String html = R"rawliteral(
      <!DOCTYPE html>
      <html>
      <head><title>Flood Monitoring</title></head>
      <body>
        <h1>Flood Monitoring System</h1>
        <p>Water Level Low: %LOW%</p>
        <p>Water Level Mid: %MID%</p>
        <p>Water Level High: %HIGH%</p>
        <p>Flow Rate: %FLOW% L/min</p>
      </body>
      </html>
    )rawliteral";

    // Replace placeholders with real data
    html.replace("%LOW%", getWaterLevel(waterLevelLow) ? "Yes" : "No");
    html.replace("%MID%", getWaterLevel(waterLevelMid) ? "Yes" : "No");
    html.replace("%HIGH%", getWaterLevel(waterLevelHigh) ? "Yes" : "No");
    html.replace("%FLOW%", String(getFlowRate(), 2));

    // Send HTML Response
    client.print("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");
    client.print(html);

    delay(1);
    client.stop();
    Serial.println("Client disconnected");
  }
}

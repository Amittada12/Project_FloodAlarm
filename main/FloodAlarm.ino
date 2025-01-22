#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Arduino_JSON.h>

// WiFi Credentials
const char* ssid = "Your_SSID";          // ใส่ชื่อ WiFi ของคุณ
const char* password = "Your_PASSWORD";  // ใส่รหัสผ่าน WiFi ของคุณ

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

// Web Server
AsyncWebServer server(80);

// JSON variable to hold sensor readings
JSONVar sensorReadings;

// Notification flag
bool enableNotifications = true;

// ISR for Flow Rate Sensor
void pulseCounter() {
  pulseCount++;
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

// Get Water Level State (return true if water is at this level or above)
bool getWaterLevel(int pin) {
  return digitalRead(pin) == LOW; // Float Switch triggers LOW when water is at the level
}

// Prepare JSON Data
String getSensorData() {
  sensorReadings["waterLevelLow"] = getWaterLevel(waterLevelLow);
  sensorReadings["waterLevelMid"] = getWaterLevel(waterLevelMid);
  sensorReadings["waterLevelHigh"] = getWaterLevel(waterLevelHigh);
  sensorReadings["flowRate"] = getFlowRate();
  return JSON.stringify(sensorReadings);
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Initialize Pins
  pinMode(waterLevelLow, INPUT_PULLUP);
  pinMode(waterLevelMid, INPUT_PULLUP);
  pinMode(waterLevelHigh, INPUT_PULLUP);
  pinMode(flowRatePin, INPUT);

  // Attach Interrupt for Flow Rate Sensor
  attachInterrupt(digitalPinToInterrupt(flowRatePin), pulseCounter, RISING);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected! IP: " + WiFi.localIP().toString());

  // Serve Web Page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "text/html", htmlPage());
  });

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
  bool levelLow = getWaterLevel(waterLevelLow);
  bool levelMid = getWaterLevel(waterLevelMid);
  bool levelHigh = getWaterLevel(waterLevelHigh);

  if (enableNotifications) {
    if (levelHigh) {
      Serial.println("Red Alert: Extreme Danger!");
    } else if (levelMid) {
      Serial.println("Orange Alert: High Risk!");
    } else if (levelLow) {
      Serial.println("Yellow Alert: Moderate Risk!");
    } else {
      Serial.println("Green: Safe Water Level");
    }
  }
}

// HTML Web Page
String htmlPage() {
  return R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Flood Monitoring</title>
  <style>
    body { font-family: Arial, sans-serif; text-align: center; background: #f4f4f9; margin: 0; padding: 20px; }
    .status { font-size: 1.5em; margin: 20px; padding: 20px; border-radius: 5px; }
    .green { background: #4caf50; color: white; }
    .yellow { background: #ffc107; color: black; }
    .red { background: #f44336; color: white; }
    .chart { margin: 30px auto; }
  </style>
</head>
<body>
  <h1>Flood Monitoring System</h1>
  <div id="status" class="status green">Safe Water Level</div>
  <div>
    <canvas id="waterLevelChart" width="400" height="200"></canvas>
  </div>
  <button onclick="toggleNotifications()">Toggle Notifications</button>
  <script>
    const statusDiv = document.getElementById('status');
    const ctx = document.getElementById('waterLevelChart').getContext('2d');
    let chart = new Chart(ctx, {
      type: 'line',
      data: { labels: [], datasets: [{ label: 'Water Level', data: [] }] },
    });

    function updateStatus(levelLow, levelMid, levelHigh) {
      if (levelHigh) {
        statusDiv.className = 'status red';
        statusDiv.textContent = 'Red Alert: Extreme Danger!';
      } else if (levelMid) {
        statusDiv.className = 'status yellow';
        statusDiv.textContent = 'Orange Alert: High Risk!';
      } else if (levelLow) {
        statusDiv.className = 'status yellow';
        statusDiv.textContent = 'Yellow Alert: Moderate Risk!';
      } else {
        statusDiv.className = 'status green';
        statusDiv.textContent = 'Green: Safe Water Level';
      }
    }

    async function fetchData() {
      const res = await fetch('/api/sensors');
      const data = await res.json();
      updateStatus(data.waterLevelLow, data.waterLevelMid, data.waterLevelHigh);
      chart.data.labels.push(new Date().toLocaleTimeString());
      chart.data.datasets[0].data.push(data.flowRate);
      chart.update();
    }

    setInterval(fetchData, 2000);

    async function toggleNotifications() {
      await fetch('/api/toggle-notifications', { method: 'POST' });
    }
  </script>
</body>
</html>
)rawliteral";
}

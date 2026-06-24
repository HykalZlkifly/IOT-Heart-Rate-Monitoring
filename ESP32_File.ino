#include <WiFi.h>
#include <HTTPClient.h>

// 1. WiFi Profile Details
const char* ssid     = "takdewifi_2.4Ghz";
const char* password = "12345678";

// 2. Local Endpoint API Route Setup (Updated to your target subnet API schema)
const char* api_endpoint = "http://192.168.0.199:3000/api/pulse";

void setup() {
  // Prints debugging traces out to your PC via standard USB connection
  Serial.begin(115200);
  
  // Setup Hardware Serial channel 2 to intercept incoming packets from STM32
  // Wire Connections: Connect STM32 PA9 (TX) to ESP32 Pin 16 (RX2)
  Serial2.begin(115200, SERIAL_8N1, 16, 17);

  Serial.println("\n--- ESP32 Receiver Node Initialized ---");
  
  WiFi.begin(ssid, password);
  Serial.print("Connecting to local access point");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nNetwork Connection Online!");
  Serial.print("ESP32 Device Node IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  static String parseBuffer = "";
  
  // Listen for incoming serial telemetry streams
  while (Serial2.available() > 0) {
    char incomingChar = (char)Serial2.read();
    
    if (incomingChar == '\n') {
      // Validate string structure against expected packet tags
      if (parseBuffer.startsWith("<BPM:") && parseBuffer.endsWith(">")) {
        int splitIndex = parseBuffer.indexOf(':');
        String extractedBpm = parseBuffer.substring(splitIndex + 1, parseBuffer.length() - 1);
        int cleanBpmInt = extractedBpm.toInt();
        
        // Output reading directly to local PC serial terminal log
        Serial.print("[DATA RECEIVED] Realtime BPM: ");
        Serial.println(cleanBpmInt);
        
        // Execute network post function out to cloud API
        transmitToCloud(cleanBpmInt);
      }
      parseBuffer = ""; // Reset parsing frame index
    } else {
      parseBuffer += incomingChar;
    }
  }
}

void transmitToCloud(int pulseValue) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient httpClient;
    
    httpClient.begin(api_endpoint);
    httpClient.addHeader("Content-Type", "application/json");
    
    // Compile clean, standard JSON object body payload
    String requestPayload = "{\"bpm\":" + String(pulseValue) + "}";
    
    int httpResponseCode = httpClient.POST(requestPayload);
    
    if (httpResponseCode > 0) {
      Serial.print("-> API Sync Success. Server Response Code: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("-> [ERROR] Post Transaction Aborted: ");
      Serial.println(httpClient.errorToString(httpResponseCode).c_str());
    }
    httpClient.end();
  } else {
    Serial.println("-> [ERROR] Post Dropped: WiFi interface offline.");
  }
}
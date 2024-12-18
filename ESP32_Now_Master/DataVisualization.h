#include "InitialBLESetup.h"
#include "LittleFS.h"
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>

const char* fileName1 = "/november_data.csv";
const char* fileName2 = "/december_data.csv";
const char* ssid = g_userData.wifiSSID;
const char* password = g_userData.wifiPassword;

// Initialize LittleFS
void initLittleFS() {
  Serial.println("Initializing LittleFS...");
  if (!LittleFS.begin()) {
    Serial.println("An error has occurred while mounting LittleFS");
  } else {
    Serial.println("LittleFS mounted successfully");
  }
}

// Initialize WiFi
void initWiFi() {
  int count=0;
  Serial.println("Initializing WiFi...");
  WiFi.setHostname("planmon");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(".");
    count++;
    if(count==20)
    {
      break;
    }
  }

  Serial.println("\nWi-Fi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Wi-Fi Channel: ");
  Serial.println(WiFi.channel()); 
  
  // Initialize mDNS
  if (!MDNS.begin("planmon")) {
    Serial.println("Error starting mDNS");
  } else {
    Serial.println("mDNS started");
  }
  
  Serial.println(WiFi.status()); 
  MDNS.addService("http", "tcp", 80);
  Serial.println(WiFi.status());
}

// Get sensor readings in JSON format
String getSensorReadings(sensorData *RxdData) {
  //Serial.println("Getting sensor readings...");

  String jsonString = "{";
  jsonString += "\"temperature\":\"" + String(RxdData->temperature) + "\",";
  jsonString += "\"humidity\":\"" + String(RxdData->humidity) + "\",";
  jsonString += "\"moisture\":\"" + String(RxdData->soilMoisture) + "\",";
  jsonString += "\"lux\":\"" + String(RxdData->lightIntensity) + "\"";
  jsonString += "}";

  Serial.println("Sensor readings fetched successfully.");
  return jsonString;
}


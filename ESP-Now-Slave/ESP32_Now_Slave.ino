// ESP32 • ESP32NOW • BASIC • SLAVE
#include <esp_now.h>
#include "main.h"
#include "DataMeasurement.h"
#include "DataStorage.h"
#include "InitialSetupBLE.h"

userDatainString g_StrUserData;
constexpr char WIFI_SSID[] = "OnePlus11R5G";
int32_t getWiFiChannel(const char *ssid);

//#define CHANNEL                1

uint8_t masterMac [6];     // masterMac [] = {0xFC, 0x71, 0xBF, 0x9D, 0xDD, 0xEC};
esp_now_peer_info_t master;
char Master_data[10];
const esp_now_peer_info_t *masterNode = &master;

uint32_t cnt = 0;
uint8_t data = 0;

// Init ESP Now with fallback
void InitESPNow() {
//  WiFi.disconnect();
  if (esp_now_init() == ESP_OK) {
    Serial.println(F("ESPNow Init Success"));
  }
  else {
    Serial.println(F("ESPNow Init Failed"));
    // Retry InitESPNow, add a counte and then restart?
    // InitESPNow();
    // or Simply Restart
    ESP.restart();
  }
}

// Check if the slave is already paired with the master.
// If not, pair the slave with master
bool manageMaster() {   
  if (master.channel == 6) {
    Serial.print(F("Master status:           "));
    // check if the peer exists
    bool exists = esp_now_is_peer_exist(master.peer_addr);
    if ( exists) {
      // Master already paired.
      Serial.println(F("Already Paired"));
      return true;
    } else {
      // Master not paired, attempt pair
      esp_err_t addStatus = esp_now_add_peer(&master);
      if (addStatus == ESP_OK) {
        // Pair success
        Serial.println(F("Pair success"));
        return true;
      } else if (addStatus == ESP_ERR_ESPNOW_NOT_INIT) {
        // How did we get so far!!
        Serial.println(F("ESPNOW Not Init"));
        return false;
      } else if (addStatus == ESP_ERR_ESPNOW_ARG) {
        Serial.println(F("Invalid Argument"));
        return false;
      } else if (addStatus == ESP_ERR_ESPNOW_FULL) {
        Serial.println(F("Peer list full"));
        return false;
      } else if (addStatus == ESP_ERR_ESPNOW_NO_MEM) {
        Serial.println(F("Out of memory"));
        return false;
      } else if (addStatus == ESP_ERR_ESPNOW_EXIST) {
        Serial.println(F("Peer Exists"));
        return true;
      } else {
        Serial.println(F("Not sure what happened"));
        return false;
      }
    }
  } else {
    // No master found to process
    Serial.println(F("No master found to process"));
    return false;
  }
}

// send data
void sendData() { 
  //ReadSensorData to be sent to Master
  readSensorData();
//#if SIMULATIONCODE == YES
  Serial.print(F("Sending to Master Device number: "));
  Serial.println(s_sensorData.deviceID);
  Serial.print(F("Sending to Master Temperature: "));
  Serial.println(s_sensorData.temperature);
  Serial.print(F("Sending to Master Humidity: "));
  Serial.println(s_sensorData.humidity);
  Serial.print(F("Sending to Master LightIntensity: "));
  Serial.println(s_sensorData.lightIntensity); 
  Serial.print(F("Sending to Master Soil Moisture: "));
  Serial.println(s_sensorData.soilMoisture);
//#endif 
  esp_err_t ret = esp_now_send(master.peer_addr, (uint8_t*)&s_sensorData, sizeof(s_sensorData));
  logSDCard();
  readingID++;

  if (ret != ESP_OK ){
    Serial.print (F("Error sending message!! "));
    if (ret == ESP_ERR_ESPNOW_NOT_INIT) Serial.println(F("ESPNOW is not initialized"));
    if (ret == ESP_ERR_ESPNOW_ARG) Serial.println(F("invalid argument"));
    if (ret == ESP_ERR_ESPNOW_INTERNAL) Serial.println(F("internal error"));
    if (ret == ESP_ERR_ESPNOW_NO_MEM) Serial.println(F("out of memory"));
    if (ret == ESP_ERR_ESPNOW_NOT_FOUND) Serial.println(F("peer is not found"));
    if (ret == ESP_ERR_ESPNOW_IF) Serial.println(F("current WiFi interface doesn’t match that of peer"));
  } else {
    Serial.print(F("Data Sent"));
  }     
}

// callback when data is sent from Master to Slave
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print(F("Last Packet Sent to:     ")); 
  Serial.println(macStr);
  Serial.print(F("Last Packet Send Status: ")); 
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  Serial.println();
}

// callback when data is recv from Master
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *r_data, int data_len) {
  char macStr[18];
  uint8_t inData;  
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], 
           mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print(F("Last Packet Recv from:   ")); 
  Serial.println(macStr);
  memcpy(&Master_data, r_data, sizeof(Master_data));
  Serial.println("received from master: " + String (Master_data));

  // add master as peer if it has not been added already
  memcpy (&master.peer_addr, mac_addr, 6);  
  bool isPaired = manageMaster();
  if (isPaired) 
  {
    // pair success or already paired, send data back to master
    sendData();
  }
   else 
   {
    // slave pair failed
    Serial.println(F("Master pair failed!"));
   }
  }


void setup() {
  Serial.begin(115200);
  
  dataMeasureInit();
  dataStorageInit();
  prefs.begin("nodedata");
  g_StrUserData.plantName = prefs.getString("plantname","");
  g_StrUserData.locData = prefs.getString("location","");
  g_StrUserData.deviceID = prefs.getString("deviceID","");
  
  if (g_StrUserData.plantName == "" || g_StrUserData.locData == "" || g_StrUserData.deviceID ==""){
    Serial.println("No values saved for previously stored data,Starting Bluetooth to take the data");
    userSetupInit();
  }
  else {
   //Do nothing
   g_StrUserData.plantName.toCharArray(s_sensorData.plantName, sizeof(s_sensorData.plantName));
   g_StrUserData.locData.toCharArray(s_sensorData.locData, sizeof(s_sensorData.locData));
   g_StrUserData.deviceID.toCharArray(s_sensorData.deviceID, sizeof(s_sensorData.deviceID));
   prefs.end();
  }
  Serial.println(F("ESPNow/Basic/Slave Node"));
  //Set device in AP mode to begin with
  WiFi.mode(WIFI_AP);
  // configure device AP mode
  const char *SSID = "Slave_1";
  bool result = WiFi.softAP(SSID, "Slave_1  _Password", getWiFiChannel(WIFI_SSID), 0);
  if (!result) {
    Serial.println(F("AP Config failed."));
  } else {
    Serial.println("AP Config Success, AP: " + String(SSID));
  }
  
  // This is the mac address of the Slave in AP Mode
  Serial.print(F("AP MAC : "));
  Serial.println(WiFi.softAPmacAddress());
  Serial.print(F("STA MAC: "));
  Serial.println(WiFi.macAddress());
   // Init ESPNow with a fallback logic
  InitESPNow();

  master.channel = getWiFiChannel(WIFI_SSID);
  Serial.print(F("master.channel assigned: "));
  Serial.println(master.channel);
  master.ifidx = WIFI_IF_STA; //Was AP here.!

  // register callback send and receive
  esp_now_register_send_cb(OnDataSent);  
  esp_now_register_recv_cb(OnDataRecv);
  Serial.println();  
}


int32_t getWiFiChannel(const char *ssid) {
  if (int32_t n = WiFi.scanNetworks()) {
    for (uint8_t i=0; i<n; i++) {
      if (!strcmp(ssid, WiFi.SSID(i).c_str())) {
          Serial.print(F("getWiFiChannel returned: "));
          Serial.println(WiFi.channel(i));
        return WiFi.channel(i);
      }
    }
  }
  return 0;
}

// Insert your SSID

void loop() {
  cnt++;
  delay(1500);
}
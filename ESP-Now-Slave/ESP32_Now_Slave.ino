// ESP32 • ESP32NOW • BASIC • SLAVE
#include <esp_now.h>
#include "main.h"
#include "DataMeasurement.h"
#include "DataStorage.h"
#include "InitialSetupBLE.h"

#define YES 1
#define NO  0
#define SIMULATIONCODE        NO

#define CHANNEL                1

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
  if (master.channel == CHANNEL) {
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
   if(String(Master_data)=="Send data")
   {
      sendData();
   } 
   else 
   {
    // slave pair failed
    Serial.println(F("Master pair failed!"));
   }
  }
}


void setup() {
  Serial.begin(115200);
  dataMeasureInit();
  dataStorageInit();
  userSetupInit();
  Serial.println(F("ESPNow/Basic/Slave Node"));
  //Set device in AP mode to begin with
  WiFi.mode(WIFI_AP);
  // configure device AP mode
  const char *SSID = "Slave_1";
  bool result = WiFi.softAP(SSID, "Slave_1  _Password", CHANNEL, 0);
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

  master.channel = CHANNEL;
  master.ifidx = WIFI_IF_AP;

  // register callback send and receive
  esp_now_register_send_cb(OnDataSent);  
  esp_now_register_recv_cb(OnDataRecv);
  Serial.println();  
}

void loop() {
  cnt++;
  delay(1500);
}
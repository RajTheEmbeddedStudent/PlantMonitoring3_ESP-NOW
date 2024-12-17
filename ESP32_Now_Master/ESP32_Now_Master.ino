//*  ESPNow - Basic communication - Master
#include <esp_now.h>
#include "DataStorage.h"
#include "DataVisualization.h"

// Global copy of slave
esp_now_peer_info_t slave[20];
#define CHANNEL 1
#define PRINTSCANRESULTS 0
#define DELETEBEFOREPAIR 0

uint32_t cnt = 0;

//Function declarations
//bool manageSlave(slave slaveData[]);
//void sendData(slave slaveData[]);

// Init ESP Now with fallback
void InitESPNow() {
  WiFi.disconnect();
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow Init Success");
  }
  else {
    Serial.println("ESPNow Init Failed");
    // Retry InitESPNow, add a counte and then restart?
    // InitESPNow();
    // or Simply Restart
    ESP.restart();
  }
}

// Scan for slaves in AP mode
void ScanForSlave() {
  int8_t scanResults = WiFi.scanNetworks();
  memset(&slave, 0, sizeof(slave));

  Serial.println("");
  if (scanResults == 0) {
    Serial.println("No WiFi devices in AP Mode found");
  } else {
    Serial.print("Found "); Serial.print(scanResults); Serial.println(" devices ");
      
    for (int i = 0; i < scanResults; ++i) {
      // Print SSID and RSSI for each device found
      String SSID = WiFi.SSID(i);
      int32_t RSSI = WiFi.RSSI(i);           // Signalstärke
      String BSSIDstr = WiFi.BSSIDstr(i);    // Basic Service Set Identification:  bezeichnet jedes BSS eindeutig, entspricht entweder der MAC-Adresse des AP oder wird ersatzweise als Zufallszahl erzeugt.

      // Check if the current device starts with `Slave`
      if (SSID.indexOf("Slave") == 0) {
        // SSID of interest
        Serial.println("Found a Slave.");
        Serial.print(i + 1); Serial.print(": "); Serial.print(SSID); Serial.print(" ["); Serial.print(BSSIDstr); Serial.print("]"); Serial.print(" ("); Serial.print(RSSI); Serial.print(")"); Serial.println("");
        // Get BSSID => Mac Address of the Slave
        int mac[6];
        if ( 6 == sscanf(BSSIDstr.c_str(), "%x:%x:%x:%x:%x:%x",  &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5] ) ) {
          for (int ii = 0; ii < 6; ++ii ) {
            slave[slaveFound].peer_addr[ii] = (uint8_t) mac[ii];
          }
        }

        slave[slaveFound].channel = CHANNEL; // pick a channel
        slave[slaveFound].encrypt = 0; // no encryption

        slaveFound = slaveFound + 1;
      }
    }
  }
  // clean up ram
  WiFi.scanDelete();
}

// Check if the slave is already paired with the master.
// If not, pair the slave with master
bool manageSlave(esp_now_peer_info_t *slaveData) {
  if (slaveData->channel == CHANNEL) {
    if (DELETEBEFOREPAIR) {
      //deletePeer();
    }

    Serial.print("Slave Status:            ");
    // check if the peer exists
    bool exists = esp_now_is_peer_exist(slaveData->peer_addr);
    if ( exists) {
      // Slave already paired.
      Serial.println("Already Paired");
      return true;
    } else {
      // Slave not paired, attempt pair
      esp_err_t addStatus = esp_now_add_peer(slaveData);
      if (addStatus == ESP_OK) {
        // Pair success
        Serial.println("Pair success");
        return true;
      } else if (addStatus == ESP_ERR_ESPNOW_NOT_INIT) {
        // How did we get so far!!
        Serial.println("ESPNOW Not Init");
        return false;
      } else if (addStatus == ESP_ERR_ESPNOW_ARG) {
        Serial.println("Invalid Argument");
        return false;
      } else if (addStatus == ESP_ERR_ESPNOW_FULL) {
        Serial.println("Peer list full");
        return false;
      } else if (addStatus == ESP_ERR_ESPNOW_NO_MEM) {
        Serial.println("Out of memory");
        return false;
      } else if (addStatus == ESP_ERR_ESPNOW_EXIST) {
        Serial.println("Peer Exists");
        return true;
      } else {
        Serial.println("Not sure what happened");
        return false;
      }
    }
  } else {
    // No slave found to process
    Serial.println("No Slave found to process");
    return false;
  }
}

/*
void deletePeer() {
  esp_err_t delStatus = esp_now_del_peer(slave.peer_addr);
  Serial.print("Slave Delete Status: ");
  if (delStatus == ESP_OK) {
    // Delete success
    Serial.println("Success");
  } else if (delStatus == ESP_ERR_ESPNOW_NOT_INIT) {
    // How did we get so far!!
    Serial.println("ESPNOW Not Init");
  } else if (delStatus == ESP_ERR_ESPNOW_ARG) {
    Serial.println("Invalid Argument");
  } else if (delStatus == ESP_ERR_ESPNOW_NOT_FOUND) {
    Serial.println("Peer not found.");
  } else {
    Serial.println("Not sure what happened");
  }
}
*/

// send data
void sendData(esp_now_peer_info_t *slaveData) {
//  data++;
//  const uint8_t *peer_addr = slave.peer_addr;
//  Serial.print("Sending: "); Serial.println(data);
//  esp_err_t result = esp_now_send(peer_addr, &data, sizeof(data));
  const uint8_t *peer_addr = slaveData->peer_addr;
  char commandtoSlave[10] = "Send data"; 
  uint8_t s_data[sizeof(commandtoSlave)];  
  memcpy(s_data, &commandtoSlave, sizeof(commandtoSlave));
  Serial.println("send to slave:           " + String (commandtoSlave));  
  // Daten in einen Speicherblock kopieren und zurück senden
  esp_err_t result = esp_now_send(peer_addr, s_data, sizeof(s_data));
      
  Serial.print("Send Status:             ");
  if (result == ESP_OK) {
    Serial.println("Success");
  } else if (result == ESP_ERR_ESPNOW_NOT_INIT) {
    // How did we get so far!!
    Serial.println("ESPNOW not Init.");
  } else if (result == ESP_ERR_ESPNOW_ARG) {
    Serial.println("Invalid Argument");
  } else if (result == ESP_ERR_ESPNOW_INTERNAL) {
    Serial.println("Internal Error");
  } else if (result == ESP_ERR_ESPNOW_NO_MEM) {
    Serial.println("ESP_ERR_ESPNOW_NO_MEM");
  } else if (result == ESP_ERR_ESPNOW_NOT_FOUND) {
    Serial.println("Peer not found.");
  } else {
    Serial.println("Not sure what happened");
  }
}

// callback when data is sent from Master to Slave
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("Last Packet Sent to:     "); Serial.println(macStr);
  Serial.print("Last Packet Send Status: "); Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// callback when data is received from Slave To Master
void onDataRecv(const uint8_t *mac_addr, const uint8_t *r_data, int data_len) {
  char macStr[18];
  sensorData s_sensorData;
  //MAC Adresse des Slaves zur Info
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print(F("Last Packet Recv from:   ")); 
  Serial.println(macStr);
  memcpy(&s_sensorData, r_data, sizeof(s_sensorData));
  //Log received data to SD card
  logSDCard(&s_sensorData);
  readingID++;

#if SIMULATIONCODE == YES
  Serial.print(F("received from slave Device number: "));
  Serial.println(s_sensorData.deviceID);
  Serial.print(F("received from slave Temperature: "));
  Serial.println(s_sensorData.temperature);
  Serial.print(F("received from slave Humidity: "));
  Serial.println(s_sensorData.humidity);
  Serial.print(F("received from slave Light Intensity: "));
  Serial.println(s_sensorData.lightIntensity); 
  Serial.print(F("received from slave SoilMoisture: "));
  Serial.println(s_sensorData.soilMoisture);
#endif 
}


void setup() {
  Serial.begin(115200);
  dataStorageInit();
  //Set device in STA mode to begin with
  WiFi.mode(WIFI_STA);
  Serial.println("ESPNow/Basic/Master");
  // This is the mac address of the Master in Station Mode
  Serial.print("STA MAC: "); Serial.println(WiFi.macAddress());
  // Init ESPNow with a fallback logic
  InitESPNow();
  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  //Callback Funktion für den Empfang registrieren 
  esp_now_register_recv_cb(onDataRecv);  
}

void loop() {
  cnt++;
  // In the loop we scan for slave
  ScanForSlave();
  // If Slave is found, it would be populate in `slave` variable
  for(int j=0;j<slaveFound;j++)
  {
    bool isPaired = manageSlave(&slave[j]);
    if (isPaired) 
    {
      // if pair success or already paired
      // Send data to device   
      sendData(&slave[j]);
    } 
    else 
    {
      // slave pair failed
      Serial.println("Slave pair failed!");
    }
  }
  slaveFound = 0;
  // wait for 3seconds to run the logic again
  delay(3000);
}
//#include "main.h"
#include "FS.h"
#include "SD.h"
#include <SPI.h>

// Define CS pin for the SD card module
#define SD_CS                  5

String dataMessage;

//Declare the functions
void writeFile(fs::FS &fs, const char * path, const char * message);
void appendFile(fs::FS &fs, const char * path, const char * message);

void dataStorageInit()
{
  // Initialize SD card
  SD.begin(SD_CS);  
  if(!SD.begin(SD_CS)) {
    Serial.println(F("Card Mount Failed"));
    return;
  }
  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE) {
    Serial.println(F("No SD card attached"));
    return;
  }
  Serial.println(F("Initializing SD card..."));
  if (!SD.begin(SD_CS)) {
    Serial.println(F("ERROR - SD card initialization failed!"));
    return;    // init failed
  }
}

// Write the sensor readings on the SD card
void logSDCard(sensorData *RxdData) {
  //File Name on the Master SD card storage depending on Slave Unique ID
  nameSlave.concat(RxdData->deviceID);
  nameSlave.concat(".csv");
  Serial.print(F("File name is"));
  Serial.println(nameSlave);
  nameSlave.toCharArray(FileName, 100);

  File file = SD.open(nameSlave);
  if(!file) 
  {
    Serial.println(F("File doens't exist"));
    Serial.println(F("Creating file..."));
    writeFile(SD, FileName, "ReadingID,Temperature,Humidity,Soil Moisture,Light Intensity \r\n"); //ReadingID to be replaced with timestamp in future
  }
  else 
  {
    Serial.println(F("File already exists")); 
  }
  file.close();

  dataMessage = String(readingID)               + ","  //In Future, this must be replaced with Timestamp
              + String(RxdData->temperature)    + "," 
              + String(RxdData->humidity)       + "," 
              + String(RxdData->lightIntensity) + "," 
              + String(RxdData->soilMoisture)   + "\r\n";
  appendFile(SD, FileName, dataMessage.c_str());
  //Once appending is successful reset the Filename string.
  nameSlave = "/";
#if SIMULATIONCODE == YES
  //Print the storing to SD Card data ; Enable for debugging only
  Serial.print(F("Saving to SD Card Device number: "));
  Serial.println(RxdData->deviceID);
  Serial.print(F("Saving to SD Card Temperature: "));
  Serial.println(RxdData->temperature);
  Serial.print(F("Saving to SD Card Humidity: "));
  Serial.println(RxdData->humidity);
  Serial.print(F("Saving to SD Card Light Intensity: "));
  Serial.println(RxdData->lightIntensity); 
  Serial.print(F("Saving to SD Card SoilMoisture: "));
  Serial.println(RxdData->soilMoisture);
#endif
}

// Write to the SD card (DON'T MODIFY THIS FUNCTION)
void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file) {
    Serial.println(F("Failed to open file for writing"));
    return;
  }
  if(file.print(message)) {
    Serial.println(F("File written"));
  } else {
    Serial.println(F("Write failed"));
  }
  file.close();
}

// Append data to the SD card (DON'T MODIFY THIS FUNCTION)
void appendFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if(!file) {
    Serial.println(F("Failed to open file for appending"));
    return;
  }
  if(file.print(message)) {
    Serial.println(F("Message appended"));
  } else {
    Serial.println(F("Append failed"));
  }
  file.close();
}
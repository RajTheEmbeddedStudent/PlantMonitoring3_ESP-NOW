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
void logSDCard() {
  //Derive the name of the file from Unique ID entered by the user.
  nameSlave.concat(s_sensorData.deviceID);
  nameSlave.concat(".txt");
  Serial.print(F("File name is"));
  Serial.println(nameSlave);
  nameSlave.toCharArray(FileName, 100);
  //Create a file with appropriate name
  File file = SD.open(nameSlave);
  if(!file) 
  {
    Serial.println(F("File doens't exist"));
    Serial.println(F("Creating file..."));
    writeFile(SD, FileName, "Reading ID, Date, Hour, Temperature \r\n");
  }
  else 
  {
    Serial.println(F("File already exists"));  
  }
  file.close();
  //Add the data into the newly created file.
  dataMessage = String(readingID) + "   |   " 
               + String("Temperature")     +  " = " + String(s_sensorData.temperature)    + String(" degree Celsius") + "   |   " 
               + String("Humidity")        +  " = " + String(s_sensorData.humidity)       + String(" %")              + "   |   " 
               + String("Light Intensity") +  " = " + String(s_sensorData.lightIntensity) + String(" Lux")            + "   |   " 
               + String("Soil Moisture")   +  " = " + String(s_sensorData.soilMoisture)   + String(" %")              + "\r\n";
  appendFile(SD, FileName, dataMessage.c_str());
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
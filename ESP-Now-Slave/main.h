#include <WiFi.h>
//Naming for the slave data
String nameSlave = "/";
char FileName[50] = {};

//Structure to hold sensor data
struct sensorData {
    char deviceID[15] = "";
    char locData[30] = "";
    char plantName[20] = "";
    float temperature = 0.0;
    float humidity = 0.0;
    float lightIntensity = 0.0;
    float soilMoisture = 0.0;
}s_sensorData;

//extern userData g_userData; // Declare global instance

// Save reading number on RTC memory
RTC_DATA_ATTR int readingID = 0;
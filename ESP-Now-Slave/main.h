#include <WiFi.h>
#include <Preferences.h>

Preferences prefs;
//Naming for the slave data
String nameSlave = "/";
char FileName[50] = {};

#define YES 1
#define NO  0
#define SIMULATIONCODE        NO

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

//Structure to hold Received data from User
typedef struct  
    {
    String deviceID = {}; //deviceID
    String locData = {};  //locData
    String plantName = {}; //plantName
    }userDatainString;


//extern userData g_userData; // Declare global instance
extern userDatainString g_StrUserData;
// Save reading number on RTC memory
RTC_DATA_ATTR int readingID = 0;
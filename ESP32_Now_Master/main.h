#include <WiFi.h>
#include <Preferences.h>

Preferences prefs;

//For Serial prints and other code to be avoided during actual run
#define YES                   1
#define NO                    0
#define SIMULATIONCODE        NO
//Naming for the slave data
String nameSlave = "/";
char FileName[50] = {};
int slaveFound = 0;

// Save reading number on RTC memory
RTC_DATA_ATTR int readingID = 0;

//Structure to hold Received data from slave
typedef struct  
    {
    char deviceID[15] = "PlanMon_8765";
    char locData[30] = "";
    char plantName[20] = "";
    float temperature = 0.0;
    float humidity = 0.0;
    float lightIntensity = 0.0;
    float soilMoisture = 0.0;         
    }sensorData;  
    
//Structure to hold Received data from User
typedef struct  
    {
    char deviceID[15] = ""; //deviceID
    char locData[40] = "";  //locData
    char wifiSSID[30] = ""; //plantName
    char wifiPassword[20] = ""; //New
    }userData;

//Structure to hold Received data from User
typedef struct  
    {
    String deviceID = {}; //deviceID
    String locData = {};  //locData
    String wifiSSID = {}; //plantName
    String wifiPassword = {}; //New
    }userDatainString;

extern userData g_userData; // Declare global instance
extern userDatainString g_StrUserData;
sensorData s_sensorData;
void logSDCard(sensorData *);
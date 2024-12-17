#include <WiFi.h>
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
    float temperature = 0.0;
    float humidity = 0.0;
    float lightIntensity = 0.0;
    float soilMoisture = 0.0;         
    }sensorData;  //Structure to hold Received data from User

typedef struct  
    {
    char deviceID[15] = ""; //Concatenate Unique ID with PlanMon_ Eg: Unique ID: 8765 ; then DEvice ID should be PlanMon_8765
    char locData[30] = "";
    char ssidUser[20] = "";
    char passwordUser[30] = "";         
    }userData;

void logSDCard(sensorData *);
#include <DHT.h>
#include <DHT_U.h>
//Device ID
#define Device_ID             10

//GPIO Pin Configurations
#define DHT_SENSOR_PIN        26
#define LDR_SENSOR_PIN        34
#define SOILMOIST_SENSOR_PIN  36
#define LED_PIN               25

//Sensor specific configurations
#define DHTTYPE DHT11
DHT dht(DHT_SENSOR_PIN, DHTTYPE);

void dataMeasureInit() 
{
  //Initialize sensor inputs
  pinMode(DHT_SENSOR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  // Initialize DHT sensor
  dht.begin();
}

void readSensorData()
{
    float sumTemp = 0.0;
    float sumHum = 0.0;
    float sumLight = 0;
    float R_L = 0;
    float lux = 0.0;
    float sumMoist = 0;
    float sensorVal = 0.0;
/*
for(int i=0;i<10;i++)
  {
    //Read Temperature data
    sumTemp = sumTemp + dht.readTemperature();
    //Read Humidity data
    sumHum = sumHum + dht.readHumidity();
    //Read Light intensity data
    sensorVal = analogRead(LDR_SENSOR_PIN);
    R_L = (((sensorVal / 4096.0) * 5) * 10000) / (1 - ((sensorVal / 4096.0) * 5) / 5);
    lux = pow(50 * 1e3 * pow(10, 0.7) / R_L, (1 / 0.7));
    sumLight = sumLight + lux;
    sensorVal = 0;
    //Read Soil Moisture data
    sensorVal = analogRead(SOILMOIST_SENSOR_PIN);
    sumMoist = sumMoist + map(sensorVal,0,4095,100,0);
  }
*/
//#if SIMULATIONCODE == YES
  //For simulating a slave sending the data without sensors
  sumTemp = 1234;
  sumHum = 5678;
  sumLight = 8765;
  sumMoist = 4321;
//#endif
  s_sensorData.temperature = sumTemp/10.0;
  s_sensorData.humidity = sumHum/10.0;
  s_sensorData.lightIntensity = sumLight; //(sumLight/10.0) * 10;
  s_sensorData.soilMoisture = sumMoist/10.0;
}

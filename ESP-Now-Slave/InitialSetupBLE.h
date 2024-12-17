#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

void ReadUserConfigData();
void userSetupInit();

BluetoothSerial SerialBT;
bool UserdataAvailable = false;

void userSetupInit() {
  while(!UserdataAvailable)
    {
      //Serial.println(F("The device started, now you can pair it with bluetooth!"));
      ReadUserConfigData();
      SerialBT.begin("Plan_Mon_SlaveModule"); //Bluetooth device name
    }
    
}

void ReadUserConfigData() {
  if (SerialBT.available()) {
    Serial.write(SerialBT.read());
    //String BLEdata = "";
    //BLEdata = SerialBT.read();
    //Serial.print(BLEdata);
    //Copy Plant Name, Plant Location & Unique ID into a char/string
    //SerialBT.end();
    UserdataAvailable = true;
  }
  delay(20);
}
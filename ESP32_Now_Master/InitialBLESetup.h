#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to enable it
#endif

void ReadUserConfigData();
void userSetupInit();
void processData(String data);
void saveUserData(const String& deviceid, const String& location, const String& ssid, const String& password);
userData g_userData; // Define the global variable

// Bluetooth serial instance
BluetoothSerial SerialBT;

// Global variables
bool UserdataAvailable = false;
String receivedData = "";

// Initialize Bluetooth and wait for user data
void userSetupInit() {
    SerialBT.begin("PlanMon_MasterModule"); // Bluetooth device name
    Serial.println(F("The device started, now you can pair it with Bluetooth!"));

    while (!UserdataAvailable) {
        ReadUserConfigData();
    }

    Serial.println(F("User data received successfully!"));
    Serial.println("Received Data: " + receivedData);

    // Process and store the user data
    processData(receivedData);
}

// Read data from Bluetooth
void ReadUserConfigData() {
    while (SerialBT.available()) {
        char incomingChar = SerialBT.read(); // Read one character
        Serial.write(incomingChar);         // Echo the character to Serial Monitor
        if (incomingChar == '$') {         // Check for end of message (newline character)
            UserdataAvailable = true;       // Mark data as complete
            SerialBT.disconnect();                 // Stop Bluetooth communication
            delay(200);
            SerialBT.end();
            delay(2000);
            break;
        }
        receivedData += incomingChar;       // Append character to buffer
    }
}

// Process received data and extract user information
void processData(String data) {
    // Print the raw data received for debugging
    Serial.println(F("Raw Data Received:"));
    Serial.println(data);

    // Replace the delimiter "*,*" with a space for easier parsing
    data.replace("*,*", "|");
    Serial.println(F("Data After Replacing Delimiters:"));
    Serial.println(data);

    // Split the string using space as a delimiter
    int firstSpace = data.indexOf('|');  // Find first space
    int secondSpace = data.indexOf('|', firstSpace + 1); // Find second space
    int thirdSpace = data.indexOf('|', secondSpace + 1);
    int endSpace = data.indexOf('|', thirdSpace + 1);

    if (firstSpace != -1 && secondSpace != -1) {
        String deviceid = data.substring(0, firstSpace);  // Extract name
        String location = data.substring(firstSpace + 1, secondSpace); // Extract location
        String ssid = data.substring(secondSpace + 1, thirdSpace);  // Extract ID
        String password = data.substring(thirdSpace + 1, endSpace);

        // Debugging: Print parsed values
        Serial.println(F("Parsed Values:"));
        Serial.println("deviceid: " + deviceid);
        Serial.println("Location: " + location);
        Serial.println("ssid: " + ssid);
        Serial.println("password: " + password);

        // Save parsed data into the userData structure
        saveUserData(deviceid, location, ssid, password);
    } else {
        Serial.println(F("Error: Failed to parse the received data correctly."));
    }
}

// Save data into the global userData structure
void saveUserData(const String& deviceid, const String& location, const String& ssid, const String& password) {
    // Copy name into wifiSSID
    deviceid.toCharArray(g_userData.deviceID, sizeof(g_userData.deviceID));
    // Copy location into locData
    location.toCharArray(g_userData.locData, sizeof(g_userData.locData));
    // Concatenate "PlanMon_" with ID and copy into deviceID
    //String fullDeviceID = "PlanMon_" + id;
    //fullDeviceID.toCharArray(g_userData.deviceID, sizeof(g_userData.deviceID));
    ssid.toCharArray(g_userData.wifiSSID, sizeof(g_userData.wifiSSID));

    password.toCharArray(g_userData.wifiPassword, sizeof(g_userData.wifiPassword));
    
    //To be stored into Non-volatile memory
    prefs.putString("ssid", ssid);
    prefs.putString("password", password);
    prefs.putString("deviceID", deviceid);
    prefs.putString("locData", location);
    prefs.end();

    // Debugging: Print the stored values
    Serial.println(F("Stored Data:"));
    Serial.println("Device ID: " + String(g_userData.deviceID));
    Serial.println("Location: " + String(g_userData.locData));
    Serial.println("SSID: " + String(g_userData.wifiSSID));
    Serial.println("password: " + String(g_userData.wifiPassword));
    
}

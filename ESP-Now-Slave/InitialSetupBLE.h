#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to enable it
#endif

void ReadUserConfigData();
void userSetupInit();
void processData(String data);
void saveUserData(const String& name, const String& location, const String& id);

// Bluetooth serial instance
BluetoothSerial SerialBT;

// Global variables
bool UserdataAvailable = false;
String receivedData = "";

// Initialize Bluetooth and wait for user data
void userSetupInit() {
    SerialBT.begin("PlanMon_SlaveModule"); // Bluetooth device name
    Serial.println(F("The device started, now you can pair it with Bluetooth!"));

    while (!UserdataAvailable) {
        ReadUserConfigData();
    }
//#if SIMULATIONCODE == YES
    Serial.println(F("User data received successfully!"));
    Serial.println("Received Data: " + receivedData);
//#endif
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
            SerialBT.disconnect();
            delay(200);
            SerialBT.end();                 // Stop Bluetooth communication
            break;
        }
        receivedData += incomingChar;       // Append character to buffer
    }
}

// Process received data and extract user information
void processData(String data) {
    // Print the raw data received for debugging
//#if SIMULATIONCODE == YES
    Serial.println(F("Raw Data Received:"));
    Serial.println(data);
//#endif

    // Replace the delimiter "*,*" with a space for easier parsing
    data.replace("*,*", "|");
//#if SIMULATIONCODE == YES
    Serial.println(F("Data After Replacing Delimiters:"));
    Serial.println(data);
//#endif
    // Split the string using space as a delimiter
    int firstSpace = data.indexOf('|');  // Find first space
    int secondSpace = data.indexOf('|', firstSpace + 1); // Find second space
    int endSpace = data.indexOf('|', secondSpace + 1);

    if (firstSpace != -1 && secondSpace != -1) {
        String name = data.substring(0, firstSpace);  // Extract name
        String location = data.substring(firstSpace + 1,secondSpace); // Extract location
        String id = data.substring(secondSpace + 1, endSpace);  // Extract ID
//#if SIMULATIONCODE == YES
        // Debugging: Print parsed values
        Serial.println(F("Parsed Values:"));
        Serial.println("Name: " + name);
        Serial.println("Location: " + location);
        Serial.println("ID: " + id);
//#endif

        // Save parsed data into the userData structure
        saveUserData(name, location, id);
    } else {
        Serial.println(F("Error: Failed to parse the received data correctly."));
    }
}

// Save data into the global userData structure
void saveUserData(const String& name, const String& location, const String& id) {
    // Copy name into plantName
    name.toCharArray(s_sensorData.plantName, sizeof(s_sensorData.plantName));
    // Copy location into locData
    location.toCharArray(s_sensorData.locData, sizeof(s_sensorData.locData));
    // Concatenate "PlanMon_" with ID and copy into deviceID
    String fullDeviceID = "PlanMon_" + id;
    fullDeviceID.toCharArray(s_sensorData.deviceID, sizeof(s_sensorData.deviceID));

    // Debugging: Print the stored values
    Serial.println(F("Stored Data:"));
    Serial.println("Device ID: " + String(s_sensorData.deviceID));
    Serial.println("Plant Name: " + String(s_sensorData.plantName));
    Serial.println("Location: " + String(s_sensorData.locData));
}

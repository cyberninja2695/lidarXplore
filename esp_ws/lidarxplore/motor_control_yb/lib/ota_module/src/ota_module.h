/*
=================================================================================
USAGE OF THE OTA FUNCTION
=================================================================================
Step 1:
After compiling the code, thE FIRMWARE is found in 
.pio/build/esp32-s3-devkitc-1/firmware.bin
---------------------------------------------------------------------------------
Step 2:
Create a file called version.txt in the same folder as firmware.bin
---------------------------------------------------------------------------------
Step 3:
This folder must be hosted using -
cd .pio/build/esp32-s3-devkitc-1
python3 -m http.server 8000
==================================================================================
==================================================================================
*/

#ifndef OTA_MODULE_H
#define OTA_MODULE_H

#include <Arduino.h>

class OTAUpdater {
public:
    OTAUpdater(const char *ssid, const char *password,
               const char *versionUrl, const char *firmwareUrl,
               int currentVersion);

    void begin();
    void updateIfAvailable();

private:
    const char *ssid;
    const char *password;
    const char *versionUrl;
    const char *firmwareUrl;
    int currentVersion;

    void connectWiFi();
    bool isUpdateAvailable(int &newVersion);
    void performOTA();
};

#endif
#include "ota_module.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>

OTAUpdater::OTAUpdater(const char *ssid, const char *password,
                       const char *versionUrl, const char *firmwareUrl,
                       int currentVersion)
    : ssid(ssid), password(password),
      versionUrl(versionUrl), firmwareUrl(firmwareUrl),
      currentVersion(currentVersion) {}

void OTAUpdater::begin() {
    connectWiFi();
}

void OTAUpdater::connectWiFi() {
    Serial.print("Connecting to WiFi");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi!");
}

bool OTAUpdater::isUpdateAvailable(int &newVersion) {
    HTTPClient http;
    http.begin(versionUrl);
    int httpCode = http.GET();
    if (httpCode != HTTP_CODE_OK) {
        Serial.printf("Version check failed: %s\n", http.errorToString(httpCode).c_str());
        http.end();
        return false;
    }

    newVersion = http.getString().toInt();
    http.end();

    Serial.printf("Current FW: %d | Available FW: %d\n", currentVersion, newVersion);
    return (newVersion > currentVersion);
}

void OTAUpdater::performOTA() {
    HTTPClient http;
    http.begin(firmwareUrl);
    int httpCode = http.GET();
    if (httpCode != HTTP_CODE_OK) {
        Serial.printf("Firmware download failed: %s\n", http.errorToString(httpCode).c_str());
        http.end();
        return;
    }

    int contentLength = http.getSize();
    WiFiClient *stream = http.getStreamPtr();

    if (contentLength <= 0) {
        Serial.println("Content length invalid");
        http.end();
        return;
    }

    if (!Update.begin(contentLength)) {
        Serial.println("Not enough space for OTA");
        http.end();
        return;
    }

    size_t written = Update.writeStream(*stream);
    if (written == contentLength) {
        Serial.println("OTA Written successfully");
    } else {
        Serial.printf("OTA Write error: written %d/%d\n", written, contentLength);
    }

    if (Update.end()) {
        if (Update.isFinished()) {
            Serial.println("OTA success, restarting...");
            delay(1000);
            ESP.restart();
        } else {
            Serial.println("OTA not finished");
        }
    } else {
        Serial.printf("OTA Error: %s\n", Update.errorString());
    }

    http.end();
}

void OTAUpdater::updateIfAvailable() {
    int newVersion = 0;
    if (isUpdateAvailable(newVersion)) {
        Serial.println("New firmware available, starting OTA...");
        performOTA();
    } else {
        Serial.println("No update needed.");
    }
}
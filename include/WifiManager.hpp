#pragma once

#include <ESP8266WiFiMulti.h>

class WifiManager {
public:
    WifiManager() {
        WiFi.persistent(false);
        WiFi.mode(WIFI_STA);
        _wifi.addAP("Kay-Ller", "Katha1203");
        _wifi.addAP("Kay-Fi", "Katha1203");
    }

    ~WifiManager() {
    }

    bool isConnected() const {
        return _isConnected;
    }

    void connect() {
        if (_wifi.run() == WL_CONNECTED) {
            Serial.print("WiFi connected: ");
            Serial.print(WiFi.SSID());
            Serial.print(" with ip: ");
            Serial.println(WiFi.localIP());
            _isConnected = true;
        }
    }

private:
    ESP8266WiFiMulti _wifi;
    bool _isConnected = false;
};

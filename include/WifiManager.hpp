#pragma once

#include <ESP8266WiFiMulti.h>

class WifiManager {
public:
    WifiManager(const ConfigRepository& configRepository) :
        _configRepository(configRepository) {
        WiFi.persistent(false);
        WiFi.mode(WIFI_STA);

        for (const auto& ap : _configRepository.aps()) {
            _wifi.addAP(ap.first.c_str(), ap.second.c_str());
        }
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
    const ConfigRepository& _configRepository;
    ESP8266WiFiMulti _wifi;
    bool _isConnected = false;
};

#pragma once

#include <ESP8266WiFiMulti.h>

#include "Config.hpp"

class WifiManager {
public:
    WifiManager(const Config& config) :
        _config(config) {
        WiFi.persistent(false);
        WiFi.mode(WIFI_STA);

        for (const auto& ap : _config.aps()) {
            LOG("add AP: " << ap.first);
            _wifi.addAP(ap.first.c_str(), ap.second.c_str());
        }
    }

    ~WifiManager() {
    }

    bool isConnected() const {
        return _isConnected;
    }

    void setup() {
        if (_wifi.run() == WL_CONNECTED) {
            Serial.print("WiFi connected: ");
            Serial.print(WiFi.SSID());
            Serial.print(" with ip: ");
            Serial.println(WiFi.localIP());
            _isConnected = true;
        }
    }

private:
    const Config& _config;
    ESP8266WiFiMulti _wifi;
    bool _isConnected = false;
};

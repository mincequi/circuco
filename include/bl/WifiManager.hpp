#pragma once

#include <ESP8266WiFiMulti.h>

#include "Config.hpp"

class WifiManager {
public:
    WifiManager(const Config& config) :
        _config(config) {
        WiFi.persistent(false);

        if (_config.aps().empty()) {
            LOG("start in access point mode");
            //WiFi.softAP("circuco", "circuco");
            WiFi.softAP("circuco", nullptr, 1, false, 8);
        } else {
            LOG("start in station mode");
            WiFi.mode(WIFI_STA);
            for (const auto& ap : _config.aps()) {
                LOG("add AP: " << ap.first);
                _wifi.addAP(ap.first.c_str(), ap.second.c_str());
            }
        }
    }

    ~WifiManager() {
    }

    bool isConnected() const {
        return _isConnected;
    }

    void loop() {
        if (WiFi.getMode() != WIFI_STA) {
            return;
        }
        if ((_wifi.run() == WL_CONNECTED) && !_isConnected) {
            LOG("connected to " << WiFi.SSID().c_str() << " with ip " << WiFi.localIP().toString().c_str());
            _isConnected = true;
        }
    }

private:
    const Config& _config;
    ESP8266WiFiMulti _wifi;
    bool _isConnected = false;
};

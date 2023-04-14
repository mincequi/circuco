#pragma once

#include <chrono>
#include <iostream>
#include <sstream>
#include <stdint.h>
#include <vector>

#include <tomlcpp.hpp>

#include <FileDataSourceInterface.hpp>
#include <Util.hpp>

// 00:00, 00:01, 00:02, 00:03, 00:04, 00:05, 00:06, 00:08,
// 00:10, 00:12, 00:15, 00:20, 00:25, 00:30, 00:40, 00:50,
// 01:00, 01:15, 01:30, 02:00, 02:30, 03:00, 04:00, 05:00, 06:00, 08:00
// 10:00, 12:00, 15:00, 20:00, 25:00, 30:00, 40:00, 50:00, 60:00
static const std::vector<int> _durations = {
      0, /*1,   2,*/  3,    4,    5,    6,    8,
     10,  12,  15,   20,   25,   30,   40,   50,
     60,  75,  90,  120,  150,  180,  240,  300,  360, 480,
    600, 720, 900, 1200, 1500, 1800, 2400, 3000, 3600
};

static const int _maxDuration = _durations.size()-1;
static const int _maxInterval = 27;

class ConfigRepository {
public:
    using Aps = std::vector<std::pair<std::string, std::string>>;

    ConfigRepository(FileDataSourceInterface& fileSystem) :
        _fileSystem(fileSystem) {
        load();
    }

    ~ConfigRepository() {
    }

    const Aps& aps() const {
        return _aps;
    }

    std::string strFrom() const {
        return toTime(_from*15);
    }

    std::string strTo() const {
        return toTime(_to*15);
    }

    std::string strDuration() const {
        return toDuration(_durations.at(_duration), "On");
    }

    uint duration() const {
        return _durations.at(_duration);
    }

    std::string strInterval() const {
        return toDuration(_durations.at(_interval)*60, "Off");
    }

    uint interval() const {
        return _durations.at(_interval)*60;
    }

    std::string strRoc() const {
        if (_roc <= 0.0f) return "Off";

        std::stringstream ss;
        ss << std::fixed << std::setprecision(1) << _roc << " °C/s";
        return ss.str();
    }

    float roc() const {
        return _roc;
    }

    void incrementFrom() {
        _from = std::min(++_from, _to-1);
    }

    void decrementFrom() {
        _from = std::max(--_from, 0);
    }

    void incrementTo() {
        _to = std::min(++_to, 95);
    }

    void decrementTo() {
        _to = std::max(--_to, _from+1);
    }

    void incrementDuration() {
        _duration = std::min( { ++_duration, _interval+17, _maxDuration } );
    }

    void decrementDuration() {
        _duration = std::max(--_duration, 0);
    }

    void incrementInterval() {
        _interval = std::min(++_interval, _maxInterval);
    }

    void decrementInterval() {
        _interval = std::max( { --_interval, _duration-17, 0 } );
    }

    void incrementRoc() {
        _roc = std::min(_roc+0.1f, 2.0f);
    }

    void decrementRoc() {
        _roc = std::max(_roc-0.1f, 0.0f);
    }

    static const size_t historySize = 60;
    static const size_t loopInterval = 1000;

private:
    void load() {
        // 1. parse file
        auto res = toml::parse(_fileSystem.config());
        if (!res.table) {
            std::cerr << "ConfigRepository> cannot parse file: " << res.errmsg << std::endl;
            return;
        }

        // 2. get aps array
        auto apArray = res.table->getArray("aps");
        if (!apArray) {
            std::cerr << "ConfigRepository> missing [aps]" << std::endl;
            return;
        }

        // 4. examine the values
        for (int i = 0; ; i++) {
            auto ap = apArray->getTable(i);
            if (!ap) break;

            auto[ssidOk, ssid] = ap->getString("ssid");
            auto[passwordOk, password] = ap->getString("password");
            if (!ssidOk || !passwordOk) continue;

            _aps.push_back({ ssid, password });
        }
    }

    void save() {
        
    }

    FileDataSourceInterface& _fileSystem;
    Aps _aps;

    bool _isDirty = false;

    int _from = 24;     // 06:00
    int _to = 72;       // 18:00
    //int _duration = 19; // 03:00m
    int _duration = 1; // 00:01m
    int _interval = 14; // 01:00h
    float _roc = 2.0f;
};

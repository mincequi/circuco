#pragma once

#include <chrono>
#include <iostream>
#include <sstream>
#include <stdint.h>
#include <vector>

#include <tomlcpp.hpp>

#include <FileDataSourceInterface.hpp>
#include <Logger.hpp>
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
static const int _durationIntervalOffset = 17;

class Config {
public:
    using Aps = std::vector<std::pair<std::string, std::string>>;

    Config(FileDataSourceInterface& fileSystem) :
        _fileSystem(fileSystem) {
    }

    ~Config() {
    }

    void setup() {
        load();
    }

    void loop(uint ts) {
        save();
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
        return toDuration(_durations.at(_dur), "On");
    }

    uint duration() const {
        return _durations.at(_dur);
    }

    std::string strInterval() const {
        return toDuration(_durations.at(_intvl)*60, "Off");
    }

    uint interval() const {
        return _durations.at(_intvl)*60;
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

    void changeFrom(int d) {
        changeValue(&_from, d, 0, _to-1);
    }

    void changeTo(int d) {
        changeValue(&_to, d, _from+1, 95);
    }

    void changeDuration(int d) {
        changeValue(&_dur, d, 0, std::min(_intvl+_durationIntervalOffset, _maxDuration));
    }

    void changeInterval(int d) {
        changeValue(&_intvl, d, std::max(_dur-_durationIntervalOffset, 0), _maxInterval);
    }

    void changeRoc(float d) {
        changeValue(&_roc, d, 0.0f, 2.0f);
    }

    static const size_t historySize = 60;
    static const size_t deviceInterval = 1000;
    static const size_t saveInterval = 10000;

private:
    template <typename T>
    T changeValue(T* v, T d, T lo, T hi) {
        const auto ret = std::clamp(*v+d, lo, hi);
        if (*v != ret) _isDirty = true;
        *v = ret;
        return ret;
    }

    void load() {
        // 1. parse file
        const auto res = toml::parse(_fileSystem.config());
        if (!res.table) {
            LOG("file parsing failed: " << res.errmsg);
            return;
        }

        // 2. get aps array
        const auto apArray = res.table->getArray("aps");
        if (!apArray) {
            LOG("access points missing");
            return;
        }

        // 2.1 examine ap values
        for (int i = 0; ; i++) {
            const auto ap = apArray->getTable(i);
            if (!ap) break;

            const auto[ssidOk, ssid] = ap->getString("ssid");
            const auto[passwordOk, password] = ap->getString("password");
            if (!ssidOk || !passwordOk) continue;

            _aps.push_back({ ssid, password });
        }

        // 3. Other values
        const auto[ok, from] = res.table->getInt("from");
        if (ok) _from = from;
        const auto[toOk, to] = res.table->getInt("to");
        if (toOk) _to = to;
        const auto[durOk, dur] = res.table->getInt("dur");
        if (durOk) _dur = dur;
        const auto[intvlOk, intvl] = res.table->getInt("intvl");
        if (intvlOk) _intvl = intvl;
        const auto[rocOk, roc] = res.table->getDouble("roc");
        if (rocOk) _roc = roc;
    }

    void save() {
        if (!_isDirty) return;

        LOG("saving config");
        _fileSystem.beginSaveConfig();

        if (!_aps.empty()) {
            _fileSystem.saveConfig("aps = [", true);
            for (size_t i = 0; i < _aps.size(); ++i) {
                _fileSystem.saveConfig("  { ssid = \"");
                _fileSystem.saveConfig(_aps.at(i).first);
                _fileSystem.saveConfig("\", password = \"");
                _fileSystem.saveConfig(_aps.at(i).second);
                _fileSystem.saveConfig("\" }");
                if (i + 1 < _aps.size()) _fileSystem.saveConfig(",");
                _fileSystem.saveConfig("", true);
            }
            _fileSystem.saveConfig("]", true);
        }

        _fileSystem.saveConfig("from = ");
        _fileSystem.saveConfig(_from, true);
        _fileSystem.saveConfig("to = ");
        _fileSystem.saveConfig(_to, true);
        _fileSystem.saveConfig("dur = ");
        _fileSystem.saveConfig(_dur, true);
        _fileSystem.saveConfig("intvl = ");
        _fileSystem.saveConfig(_intvl, true);
        _fileSystem.saveConfig("roc = ");
        _fileSystem.saveConfig(_roc, true);

        _fileSystem.endSaveConfig();

        _isDirty = false;
    }

    FileDataSourceInterface& _fileSystem;
    Aps _aps;

    bool _isDirty = false;

    int _from = 24;     // 06:00
    int _to = 72;       // 18:00
    //int _duration = 19; // 03:00m
    int _dur = 1; // 00:01m
    int _intvl = 14; // 01:00h
    float _roc = 2.0f;
};

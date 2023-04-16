#pragma once

#include <iostream>

#include <tinja.hpp>

#include <Config.hpp>
#include <FileDataSource.hpp>
#include <io/DeviceManager.hpp>

class HtmlRenderer {
public:
    HtmlRenderer(const Config& config,
                 const AbstractSensor<int16_t>& sensor,
                 const AbstractActuator& actuator,
                 const DeviceManager& deviceManager,
                 const FileDataSource& fileSystem) :
        _config(config),
        _sensor(sensor),
        _actuator(actuator),
        _deviceManager(deviceManager),
        _fileSystem(fileSystem) {
    }

    ~HtmlRenderer() {
    }

    void setup() {
        const auto tokenCount = _templ.parse(_fileSystem.htmlTemplate());
        std::cout << "HTML template tokenized to " << tokenCount << " tokens" << std::endl;
    }

    void render() {
        _data["tm"] = time();

        _data["f"] = _config.strFrom();
        _data["t"] = _config.strTo();
        _data["d"] = _config.strDuration();
        _data["i"] = _config.strInterval();
        _data["r"] = _config.strRoc();

        _data["v"] = _sensor.strValue();
        _data["p"] = std::to_string(_sensor.percentage());
        _data["dv"] = _sensor.strRoc();
        _data["dp"] = std::to_string(_sensor.deltaPercentage());

        static const std::map<bool, std::string> rocMap {
            { false, "bg-primary" },
            { true, "bg-warning" }
        };
        _data["rc"] = tinja::StringRef { rocMap.at(_sensor.roc() >= _config.roc()) };

        static const std::map<bool, std::string> isActiveMap {
            { false, "" },
            { true, "bg-warning progress-bar-striped progress-bar-animated" }
        };
        _data["ac"] = tinja::StringRef { isActiveMap.at(_actuator.isActive()) };

        // Stats data
        _data["fh"] = std::to_string(ESP.getFreeHeap());
        _data["fb"] = std::to_string(ESP.getMaxFreeBlockSize());
        _data["hf"] = std::to_string(ESP.getHeapFragmentation());

        // Graph data
        _data["maxH"] = std::to_string(_sensor.maxH());

        static const std::map<bool, std::string> colors {
            { false, "" },
            { true, "--color: #f39c12" }
        };
        std::vector<std::string> sensorHistory;
        tinja::StringRefs actuatorHistory;
        for (size_t i = 0; i < _deviceManager.sensorHistory().size(); ++i) {
            sensorHistory.emplace_back(std::to_string(_deviceManager.sensorHistory().at(i)));
            actuatorHistory.emplace_back(colors.at(_deviceManager.actuatorHistory().at(i)));
        }
        _data["sh"] = sensorHistory;
        _data["ah"] = actuatorHistory;

        _templ.renderTo(_data, _tokens);
    }

    const tinja::Template::Tokens& tokens() const {
        return _tokens;
    }

private:
    static std::string time() {
        struct tm now;
        getLocalTime(&now);
        char s[6];
        strftime(s, sizeof(s), "%R", &now);
        return s;
    }

    const Config& _config;
    const AbstractSensor<int16_t>& _sensor;
    const AbstractActuator& _actuator;
    const DeviceManager& _deviceManager;
    const FileDataSource& _fileSystem;

    tinja::Template _templ;
    mutable tinja::DataMap _data;
    mutable tinja::Template::Tokens _tokens;
};

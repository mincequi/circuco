#pragma once

#include <bl/Config.hpp>
#include <bl/DeviceManager.hpp>

#include "HtmlRendererBase.hpp"

class HtmlRendererMain : public HtmlRendererBase {
public:
    HtmlRendererMain(
        const std::string& templ,
        const Config& config,
        const SensorBase<int16_t>& sensor,
        const ActuatorBase& actuator,
        const DeviceManager& deviceManager,
        const TimeBase& time) :
        HtmlRendererBase(templ),
        _config(config),
        _sensor(sensor),
        _actuator(actuator),
        _deviceManager(deviceManager),
        _time(time) {
    }

private:
    void doRender(tinja::DataMap& data) const override {
        data["tm"] = _time.strTime();

        data["up"] = std::to_string(_config.daysUp());
        data["f"] = _config.strFrom();
        data["t"] = _config.strTo();
        data["d"] = _config.strDuration();
        data["i"] = _config.strInterval();
        data["r"] = _config.strRoc();

        data["v"] = _sensor.strValue();
        data["p"] = std::to_string(_sensor.percentage());
        data["dv"] = _sensor.strRoc();
        data["dp"] = std::to_string(_sensor.deltaPercentage());

        static const std::map<bool, std::string> rocMap {
            { false, "bg-primary" },
            { true, "bg-warning" }
        };
        data["rc"] = tinja::StringRef { rocMap.at(_sensor.roc() >= _config.roc()) };

        static const std::map<bool, std::string> isActiveMap {
            { false, "" },
            { true, "bg-warning progress-bar-striped progress-bar-animated" }
        };
        data["ac"] = tinja::StringRef { isActiveMap.at(_actuator.isActive()) };

        // Stats data
        data["fh"] = std::to_string(ESP.getFreeHeap());
        data["fb"] = std::to_string(ESP.getMaxFreeBlockSize());
        data["hf"] = std::to_string(ESP.getHeapFragmentation());

        // Graph data
        data["maxH"] = std::to_string(_sensor.maxH());

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
        data["sh"] = sensorHistory;
        data["ah"] = actuatorHistory;
    }

    const Config& _config;
    const SensorBase<int16_t>& _sensor;
    const ActuatorBase& _actuator;
    const DeviceManager& _deviceManager;
    const TimeBase& _time;
};

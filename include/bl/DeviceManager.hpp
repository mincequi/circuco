#pragma once

#include <deque>

#include <io/ActuatorBase.hpp>
#include <io/SensorBase.hpp>

#include "Config.hpp"

class DeviceManager {
public:
    DeviceManager(const Config& config,
                  SensorBase<int16_t>& sensor,
                  ActuatorBase& actuator) :
        _config(config),
        _sensor(sensor),
        _actuator(actuator) {
        _sensorHistory.resize(config.historySize, 0);
        _actuatorHistory.resize(config.historySize, false);
    }

    auto loop(uint ts) {
        _sensor.loop(ts);
        if (_sensor.roc() >= _config.roc()) {
            // Pump gets activated due to ROC
            _actuator.setActive(ts);
        } else if (_actuator.lastActiveSinceTsMs() + _config.intervalMs() <= ts) {
            // Pump gets activated due to timer interval
            _actuator.setActive(ts);
        } else if (_actuator.lastActiveSinceTsMs() + _config.durationMs() <= ts) {
            // Pump gets deactivared due to duration
            _actuator.setActive(0, false);
        }

        _sensorHistory.push_back(_sensor.value());
        _actuatorHistory.push_back(_actuator.isActive());

        while (_sensorHistory.size() > _config.historySize) {
            _sensorHistory.pop_front();
        }
        while (_actuatorHistory.size() > _config.historySize) {
            _actuatorHistory.pop_front();
        }
    }

    auto sensorHistory() const {
        std::vector<int16_t> vec;
        vec.reserve(_config.historySize);
        for (const auto& h : _sensorHistory) {
            vec.push_back(h - _sensor.minH());
        }
        return vec;
    }

    const auto& actuatorHistory() const {
        return _actuatorHistory;
    }

private:
    const Config& _config;
    SensorBase<int16_t>& _sensor;
    ActuatorBase& _actuator;

    std::deque<int16_t> _sensorHistory;
    std::deque<bool> _actuatorHistory;
};

#pragma once

#include <AbstractActuator.hpp>
#include <AbstractSensor.hpp>
#include <Config.hpp>

#include <deque>

class DeviceManager {
public:
    DeviceManager(const Config& config,
                  AbstractSensor<int16_t>& sensor,
                  AbstractActuator& actuator) :
        _config(config),
        _sensor(sensor),
        _actuator(actuator) {
        _sensorHistory.resize(config.historySize, 0);
        _actuatorHistory.resize(config.historySize, false);
    }

    auto loop(uint ts) {
        _sensor.loop(ts);
        _actuator.loop(ts);
        if (_sensor.roc() >= _config.roc()) {
            _actuator.setActive(ts + _config.duration() * 1000);
        } else if (_actuator.lastActiveUntilMs() + (_config.interval() - _config.duration()) * 1000 <= ts) {
            _actuator.setActive(ts + _config.duration() * 1000);
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
    AbstractSensor<int16_t>& _sensor;
    AbstractActuator& _actuator;

    std::deque<int16_t> _sensorHistory;
    std::deque<bool> _actuatorHistory;
};

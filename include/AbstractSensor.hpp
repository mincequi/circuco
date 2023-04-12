#pragma once

#include <array>
#include <cstdint>
#include <deque>
#include <iomanip>
#include <math.h>
#include <sstream>
#include <string>

template<class T>
class AbstractSensor {
public:
    void setup() {
        doSetup();    
    }

    void loop(uint ts) {
        const auto prevValue = _value;
        const auto prevTs = _ts;

        _value = doMeasure();
        _ts = ts;
        _minValue = std::min(_minValue, _value);
        _maxValue = std::max(_maxValue, _value);

        if (prevValue >= 0 && prevTs) {
            _roc = 1000.0f / 128.0f * (_value - prevValue) / (int)(ts - prevTs);
            _minRoc = std::min(_minRoc, _roc);
            _maxRoc = std::max(_maxRoc, _roc);
        }
    }

    T value() const {
        return _value;
    }

    std::string strValue() const {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(1) << _value/128.0f << unit();
        return ss.str();
    }

    uint percentage() const {
        return 40 + 60 * (_value - _minValue) / (_maxValue - _minValue);
    }

    float roc() const {
        return _roc;
    }

    std::string strRoc() const {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(1) << _roc << unit();
        return ss.str();
    }

    uint deltaPercentage() const {
        // TODO: (_maxRoc - _minRoc) might be 0
        return std::max(40 + 60 * (_roc - _minRoc) / (_maxRoc - _minRoc), 0.0f);
    }

    int minH() const {
        return _minValue;
    }

    int maxH() const {
        return _maxValue - _minValue;
    }

protected:
    virtual void doSetup() = 0;
    virtual T doMeasure() = 0;
    virtual std::string unit() const = 0;

private:
    uint _ts = 0;
    T _value = 0;
    T _minValue = std::numeric_limits<T>::max();
    T _maxValue = std::numeric_limits<T>::min();

    float _roc = 0;
    float _minRoc = std::numeric_limits<T>::max();
    float _maxRoc = 0;
};

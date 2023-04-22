#pragma once

#include "SensorBase.hpp"

#include <OneWire.h>
#include <DallasTemperature.h>

class SensorTemperatureDs18b20 : public SensorBase<int16_t> {
public:
    SensorTemperatureDs18b20() :
        _oneWire(D5),
        _sensor(&_oneWire) {
        _sensor.setWaitForConversion(false);
        _sensor.setResolution(12);
    }

private:
    virtual void doSetup() override {
        _sensor.begin();
    }

    virtual int16_t doMeasure() override {
        if (_mode == Mode::Demo)
            return (rand() % 256 + 6400);

        int16_t tempC = 0;
        if (_sensor.isConversionComplete()) {
            DeviceAddress deviceAddress;
	        if (!_sensor.getAddress(deviceAddress, 0)) {
		        return 0;
	        }
            tempC = _sensor.getTemp((uint8_t*)deviceAddress);
            _sensor.requestTemperatures();
        }
        return tempC;
    };

    virtual std::string unit() const override {
        return " °C";
    };

    enum class Mode {
        Demo,
        Regular
    };

    OneWire _oneWire;
    DallasTemperature _sensor;
    Mode _mode = Mode::Regular;
};

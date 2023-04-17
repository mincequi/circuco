#pragma once

#include <Arduino.h>

#include "TimeBase.hpp"

class Time : public TimeBase {
private:
    virtual void doLoop() override {
        struct tm now;
        getLocalTime(&now);
        char s[6];
        strftime(s, sizeof(s), "%R", &now);
        _time = now.tm_hour * 60 + now.tm_min;
        _strTime = s;
    }
};
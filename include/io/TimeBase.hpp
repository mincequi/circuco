#pragma once

#include <functional>
#include <string>

class TimeBase {
public:
    void loop() {
        doLoop();
    }

    int time() const {
        return _time;
    }

    std::reference_wrapper<const std::string> strTime() const {
        return _strTime;
    }

protected:
    virtual void doLoop() = 0;

    int _time = 0;
    std::string _strTime;
};

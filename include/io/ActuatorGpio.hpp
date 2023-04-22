#pragma once

#include "ActuatorBase.hpp"

class ActuatorGpio : public ActuatorBase {
protected:
    virtual void doSetActive(bool active) const override {
        digitalWrite(LED_BUILTIN, active ? HIGH : LOW);
    }
};

#pragma once

#include "AbstractActuator.hpp"

class GpioActuator : public AbstractActuator {
protected:
    virtual void doSetActive(bool active) const override {
    }
};

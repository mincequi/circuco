#ifndef GPIOACTUATOR_HPP
#define GPIOACTUATOR_HPP

#include <AbstractActuator.hpp>

class GpioActuator : public AbstractActuator {
protected:
    virtual void doSetActive(bool active) const override {
    }
};

#endif // GPIOACTUATOR_HPP

#ifndef ABSTRACTACTUATOR_HPP
#define ABSTRACTACTUATOR_HPP

#include <array>
#include <deque>
#include <string>

class AbstractActuator {
public:
    bool isActive() const {
        return _isActive;
    }

    void loop(uint ts) {
        if (ts >= _activeUntilMs) {
            _isActive = false;
            doSetActive(false);
        }
    }

    void setActive(uint activeUntilMs, bool active = true) {
        if (_isActive == active) return;

        _activeUntilMs = activeUntilMs;
        _isActive = active;
        doSetActive(active);
    }

    uint lastActiveUntilMs() const {
        return _activeUntilMs;
    }

protected:
    virtual void doSetActive(bool active) const = 0;

private:
    bool _isActive = false;
    uint _activeUntilMs = 0;
};

#endif // ABSTRACTACTUATOR_HPP

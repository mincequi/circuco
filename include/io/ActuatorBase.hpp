#pragma once

#include <cstdint>

class ActuatorBase {
public:
    void setActive(uint32_t tsMs, bool active = true) {
        if (_isActive == active) return;

        _isActive = active;
        _lastActiveSinceTsMs = active ? tsMs : _lastActiveSinceTsMs;
        doSetActive(active);
    }

    bool isActive() const {
        return _isActive;
    }

    uint32_t lastActiveSinceTsMs() const {
        return _lastActiveSinceTsMs;
    }

protected:
    virtual void doSetActive(bool active) const = 0;

private:
    bool _isActive = false;
    uint32_t _lastActiveSinceTsMs = 0;
};

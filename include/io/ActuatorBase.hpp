#pragma once

class ActuatorBase {
public:
    void setActive(uint tsMs, bool active = true) {
        if (_isActive == active) return;

        _isActive = active;
        _lastActiveSinceTsMs = active ? tsMs : _lastActiveSinceTsMs;
        doSetActive(active);
    }

    bool isActive() const {
        return _isActive;
    }

    uint lastActiveSinceTsMs() const {
        return _lastActiveSinceTsMs;
    }

protected:
    virtual void doSetActive(bool active) const = 0;

private:
    bool _isActive = false;
    uint _lastActiveSinceTsMs = 0;
};

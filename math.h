#pragma once

#include <cmath>
#include <ctime>

class SimpleEaseIn {
public:
    SimpleEaseIn() = default;
    void setValue(clock_t _duration, double _startValue, double _endValue);
    SimpleEaseIn(clock_t _duration, double _startValue, double _endValue);
    void start();
    double get();
    void clear();
protected:
    clock_t duration = 0;
    clock_t startTime = -1;
    double startValue = 0.0;
    double endValue = 0.0;
};

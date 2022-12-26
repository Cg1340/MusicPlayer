#include "math.h"


void SimpleEaseIn::setValue(clock_t _duration, double _startValue, double _endValue) {
    this->duration = _duration;
    this->startValue = _startValue;
    this->endValue = _endValue;
}

SimpleEaseIn::SimpleEaseIn(clock_t _duration, double _startValue, double _endValue) {
    this->duration = _duration;
    this->startValue = _startValue;
    this->endValue = _endValue;
}

void SimpleEaseIn::start() {
    if (this->startTime != -1) return;
    else this->startTime = clock();
}

double SimpleEaseIn::get() {
    clock_t now = clock();
    if (now - this->startTime > this->duration) {
        return endValue;
    }
    else {
        return (startValue < endValue ? startValue + \
            pow((double)(now - this->startTime) / (double)duration, 2.0) * (abs(endValue - startValue))
            : startValue - \
            pow((double)(now - this->startTime) / (double)duration, 2.0) * (abs(endValue - startValue)));

    }
}

void SimpleEaseIn::clear() {
    this->startTime = -1;
}

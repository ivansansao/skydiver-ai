#include "force.hpp"

#include <iostream>

Force::Force() {
}
Force::Force(float value, float max, float min, float ratio)
    : value(value), max(max), min(min), ratio(ratio) {
    this->originalValue = value;
}

float Force::increase() {
    const float oldValue = this->value;

    this->value += this->ratio;
    // std::cout << "value:" << this->value << " max: " << this->max << "\n";

    if (this->value > this->max) {
        this->value = this->max;
    }

    return oldValue;
}
float Force::decrease() {
    const float oldValue = this->value;

    this->value -= this->ratio;
    if (this->value < this->min) this->value = this->min;

    return oldValue;
}
void Force::reset() {
    this->value = this->originalValue;
}
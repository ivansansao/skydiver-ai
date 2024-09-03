#pragma once

#ifndef FORCE_H
#define FORCE_H

class Force {
   public:
    Force();

    Force(float value, float max, float min, float ratio);

    float value = 0.0;
    float max = 0.0;
    float min = 0.0;
    float ratio = 0.0;
    float originalValue = 0.0;

    void increase();
    void decrease();
    void reset();
};
#endif

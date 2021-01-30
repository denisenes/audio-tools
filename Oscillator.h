//
// Created by charlie on 24.10.2020.
//

#ifndef SOUNDLIB_OSCILLATOR_H
#define SOUNDLIB_OSCILLATOR_H

#include "Wave.h"
#include <cstdint>
#include <cmath>
#include <cstring>


class Oscillator {
public:
    Oscillator(const char * waveType) {
        type[0] = waveType[0];
        type[1] = waveType[1];
        type[2] = waveType[2];
    }

    int generateWave(Wave * wave, uint32_t freq);

private:
    char type[3];

    int32_t sgn(double x) {
        return x >= 0 ? INT32_MAX : -(INT32_MAX);
    }

    double mSine(double index, double frequency) {
        return sin(index * frequency);
    }

    double mSaw(double index, double frequency) {
        return 2 * (index * frequency - floor(index * frequency )) - 1;
    }
};


#endif //SOUNDLIB_OSCILLATOR_H

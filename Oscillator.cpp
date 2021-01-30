//
// Created by charlie on 24.10.2020.
//

#include "Oscillator.h"
#include "soundlib.h"

// TODO duration is double value
// Generator of simple waves (depth of sound is default = 32-bit)
int Oscillator::generateWave(Wave * wave, uint32_t freq) {
    
    DataType * data = wave->getData();
    size_t dataBytes = wave->getDataSize();
    uint32_t sRate = wave->getSampleRate();
    
    size_t size = sizeof(DataType);
    if (memcmp(type, "sin", 3) == 0) {
        for (int i = 0; i < dataBytes / size; i++) {
            data[i] = (uint32_t) (INT32_MAX / 2) * mSine(i, 2*M_PI * freq / sRate);
        }
    } else if (memcmp(type, "saw", 3) == 0) { //TODO
        for (int i = 0; i < dataBytes / size; i++) {
            data[i] = (uint32_t) (INT32_MAX / 2) * mSaw(i, 2*M_PI * freq / sRate);
        }
    } else if (memcmp(type, "sqr", 3) == 0) {
        for (int i = 0; i < dataBytes / size; i++) {
            data[i] = sgn(mSine(i, 2*M_PI * freq / sRate)) / 2;
        }
    } else if (memcmp(type, "tri", 3) == 0) {
        for (int i = 0; i < dataBytes / size; i++) {
            data[i] = (uint32_t) (INT32_MAX / 2) * (2/M_PI) * asin(mSine(i, M_PI* freq / sRate));
        }
    }

    return 0;
}

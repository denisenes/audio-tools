//
// Created by charlie on 23.10.2020.
//

#ifndef SOUNDLIB_WAVE_H
#define SOUNDLIB_WAVE_H

#include <cstdint>
#include <malloc.h>
#include "soundlib.h"

typedef struct waveHeader_m {
    // Sample rate
    uint32_t sampleRate;
    uint16_t bitsPerSample;
    // numSamples * numChannels * bitsPerSample/8
    // Countness of bytes in data
    uint32_t dataSize;
} WaveHeader;


class Wave {
public:
//TODO depth
    Wave(uint32_t sNumber, uint32_t sRate, uint16_t depth);

    uint32_t getSampleRate() const;

    uint16_t getBitsPerSample() const;

    uint32_t getDataSize() const;

    DataType * getData();

private:
    ///HEADER
    WaveHeader description;

    ///DATA
    DataType * data;
};

#endif //SOUNDLIB_WAVE_H

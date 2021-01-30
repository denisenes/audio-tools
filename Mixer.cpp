//
// Created by charlie on 24.10.2020.
//

#include "Mixer.h"
#include <cstdarg>
#include <malloc.h>

// mix N waves
Wave * Mixer::mix(int64_t N, ...) {
    va_list waves;
    va_start(waves, N);
    Wave * firstWave = va_arg(waves, Wave *);
    uint32_t size = firstWave->getDataSize() / (firstWave->getBitsPerSample() / 8);

    Wave * newWave = (Wave *) malloc(sizeof(Wave));
    *newWave = Wave(size , firstWave->getSampleRate(), firstWave->getBitsPerSample());

    DataType * newData = newWave->getData();
    DataType * oldData = firstWave->getData();
    // Move data from first struture
    for (int i = 0; i < size; i++) {
        newData[i] = (oldData[i] / N);
    }

    // Add other waves
    for (int i = 0; i < N-1; i++) {
        Wave * currentWave = va_arg(waves, Wave *);
        uint32_t currentSize = currentWave->getDataSize() / (currentWave->getBitsPerSample() / 8);
        oldData = currentWave->getData();
        if (currentSize < size) {
            newData = (DataType *) realloc(newData, currentSize);
            size = currentSize;
        }
        // Plus plus plus plus...
        for (int j = 0; j < size; j++) {
            newData[j] += (oldData[j] / N);
        }
    }
    va_end(waves);
    return newWave;
}

// mix only 2 waves
void Mixer::mix2(Wave * wave1, Wave * wave2) {
    uint32_t size = wave1->getDataSize() / (wave1->getBitsPerSample() / 8);
    DataType * dataTo = wave1->getData();
    DataType * dataFrom = wave2->getData();
    // Plus plus plus plus...
    for (int j = 0; j < size; j++) {
        dataTo[j] /= 2;
        dataTo[j] += (dataFrom[j] / 2);
    }
}

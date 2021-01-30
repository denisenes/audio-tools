//
// Created by charlie on 23.10.2020.
//

#include "Wave.h"

Wave::Wave(uint32_t sNumber, uint32_t sRate, uint16_t depth) {
    description.sampleRate = sRate;
    description.bitsPerSample = depth;
    description.dataSize = sNumber * description.bitsPerSample / 8;

    data = (DataType *) malloc(description.dataSize);
};

uint32_t Wave::getSampleRate() const {
    return description.sampleRate;
}

uint16_t Wave::getBitsPerSample() const {
    return description.bitsPerSample;
}

//get size of Data in bytes
uint32_t Wave::getDataSize() const {
    return description.dataSize;
}

DataType * Wave::getData() {
    return data;
}
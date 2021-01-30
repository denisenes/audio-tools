//
// Created by charlie on 24.10.2020.
//

#include "ADSR.h"

// simple linear ADSR volume envelope (mono)
// 0 <= a, d, s, r <= 100 (in percents)
// 0 <= susVal <= 1
void ADSR::make(Wave * wave, double attack, double decay, double sustain, double susVal, double release) {
    //check arguments
    uint32_t dataSize = wave->getDataSize();
    DataType * data = wave->getData();

    if (attack + decay + sustain + release != 100) { //mega cool condition :3
        return;
    }
    if (susVal < 0 || susVal > 1) {
        return;
    }

    int size = dataSize / (wave->getBitsPerSample() / 8);
    // build envelope wave
    //freopen("log.txt", "w", stdout); //debug
    double envelopeWave[size];
    for (int i = 0; i < size; i++) {
        envelopeWave[i] = 1;
    }
    double attackLength = size / 100 * attack;
    for (int i = 0; i < attackLength; i++) { //attack zone
        envelopeWave[i] = (1 * i / attackLength);
        //printf("%lf\n", envelopeWave[i]); //debug
    }
    double decayLength = size / 100 * decay;
    for (int i = attackLength; i < attackLength + decayLength; i++) {
        envelopeWave[i] = (susVal-1)*(i - attackLength)/decayLength + 1;
    }
    double sustainLength = size / 100 * sustain;
    for (int i = attackLength + decayLength; i < attackLength + decayLength + sustainLength; i++) {
        envelopeWave[i] = susVal;
    }
    double releaseLength = size / 100 * release;
    for (int i = attackLength + decayLength + sustainLength; i < attackLength + decayLength + sustainLength + releaseLength; i++) {
        envelopeWave[i] = (-susVal *(i - (attackLength + decayLength + sustainLength)))/releaseLength + susVal;
        //printf("%lf\n", envelopeWave[i]); //debug
    }

    // modulate amplitude of the input wave using the envelope wave
    for (int i = 0; i < size; i++) {
        data[i] = (int32_t) ((double) data[i] * envelopeWave[i]);
    }
}
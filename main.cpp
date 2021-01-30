#include <iostream>

#include "soundlib.h"
#include "WavFile.h"
#include "Wave.h"
#include "Oscillator.h"
#include "Mixer.h"
#include "ADSR.h"

int main() {

    Wave wave1 = Wave(DEFAULT_SAMPLE_RATE * 2, DEFAULT_SAMPLE_RATE, DEFAULT_DEPTH);
    Wave wave2 = Wave(DEFAULT_SAMPLE_RATE * 2, DEFAULT_SAMPLE_RATE, DEFAULT_DEPTH);
    Wave wave3 = Wave(DEFAULT_SAMPLE_RATE * 2, DEFAULT_SAMPLE_RATE, DEFAULT_DEPTH);

    Oscillator oscillator = Oscillator("tri");

    oscillator.generateWave(&wave1, 230);
    oscillator.generateWave(&wave2, 110);
    oscillator.generateWave(&wave3, 55);

    Wave * wave = Mixer::mix(3, &wave1, &wave2, &wave3);
    //Mixer::mix2(&wave1, &wave2);

    ADSR::make(wave, 50, 0, 0, 100, 50);

    WavFile wavFile = WavFile(wave);
    wavFile.wavWrite("output.wav");

    return 0;
}

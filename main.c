#include <stdio.h>
#include <stdint.h>

#include "soundlib.h"

//FUNCTION DEMONSTRATION

int main() {
    
    WavFile * wave1 = generateWave(44100, 4, "sin", 90);
    WavFile * wave2 = generateWave(44100, 4, "tri", 220);
    WavFile * wave3 = generateWave(44100, 4, "sin", 310);
    WavFile * wave4 = generateWave(44100, 4, "sin", 4046);
    WavFile * wave5 = generateWave(44100, 4, "sin", 549);
    WavFile * wave6 = generateWave(44100, 4, "tri", 3062);
    WavFile * wave7 = generateWave(44100, 4, "sin", 733);
    WavFile * wave8 = generateWave(44100, 4, "sin", 855);
    WavFile * wave9 = generateWave(44100, 4, "sin", 994);
    WavFile * wave10 = generateWave(44100, 4, "sin", 1011);
    WavFile * wave11 = generateWave(44100, 4, "sin", 2123);

    WavFile * addition = mixer(11, wave1, wave2, wave3, wave4, wave5, wave6, wave7, wave8, wave9, wave10, wave11);

    
    //WavFile * wave3 = generateWave(44100, 5, "saw", 440);

    //for (int i = 0; i < wave->header.subchunk2Size / (wave->header.bitsPerSample / 8); i++) {
    //    wave->data[i].dMode64 = wave->data[i].dMode64 + wave2->data[i].dMode64;
    //}

    ADSR(addition, 30, 25, 20, 1, 25);

    wavWrite(addition);

    return 0;
}

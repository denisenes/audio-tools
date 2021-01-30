//
// Created by charlie on 23.10.2020.
//

#ifndef SOUNDLIB_WAVFILE_H
#define SOUNDLIB_WAVFILE_H

#include <cstdint>
#include "Wave.h"
#include <malloc.h>

// WAV header
typedef struct WavHeader_m {
    // Contains symbols "RIFF" in ASCII code system
    // 0x52494646 in big-endian
    uint8_t chunkId[4];
    // 36 + subchunk2Size or:
    // Size of file after this position.
    uint32_t chunkSize;
    // Contains symbols "WAVE"
    // 0x57415645 in big-endian
    uint8_t format[4];
    // Contains symbols "fmt"
    // 0x666d7420 in big-endian
    uint8_t subchunk1Id[4];
    // 16 for PCM.
    // Header's bytes which are contained after this position.
    uint32_t subchunk1Size;
    // For PCM = 1 (Linear quantification).
    // If not equal to 1 then there is a compression
    uint16_t audioFormat;
    // Number of channels
    uint16_t numChannels;
    // Sample rate
    uint32_t sampleRate;
    // sampleRate * numChannels * bitsPerSample/8
    uint32_t byteRate;
    // numChannels * bitsPerSample/8
    // Number of bytes per one sample (including several channels)
    uint16_t blockAlign;
    // Depth of sound
    uint16_t bitsPerSample;
    // Contains "data"
    // 0x64617461
    uint8_t subchunk2Id[4];
    // numSamples * numChannels * bitsPerSample/8
    // Countness of bytes in data
    uint32_t subchunk2Size;
    //Then data goes

} WavHeader;

//  [            Sample            ]
//  [   channel1   ][   channel2   ]
//  [b1][b2][b3][b4][b1][b2][b3][b4]

class WavFile {
public:
    WavFile(uint32_t sNumber, uint32_t sRate, uint16_t depth, uint16_t channels);
    WavFile(char *fileName);
    WavFile(Wave * wave);

    int wavWrite(const char * filepath);
    int rawWrite(char * filename);
    void readRaw(char * fName);

    ~WavFile() {
        free(data);
    }

private:
    /// HEADER
    WavHeader header;

    void initHeader();

    ///DATA
    DataType *data;

};


#endif //SOUNDLIB_WAVFILE_H

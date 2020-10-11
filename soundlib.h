#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <string.h>
#include <malloc.h>
#include <stdarg.h>

#ifndef _MINITOOLS_GUARD_
#define _MINITOOLS_GUARD_

#define DEPTH 32
#define CHANNELS 1

FILE * input;
FILE * output;

typedef union SampleSize_m {
    char bytes[8];
    int32_t dMode32;
    int64_t dMode64;
    int16_t dMode16;
} SampleType;

// WAV header
typedef struct WavHeader_m
{
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
    // Frequency
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

// Here we contain header and data(array of integers) from wav file
typedef struct wavFile_m {
    WavHeader header;
    SampleType * data;
} WavFile;

// Init wav-structure with linear quantification
// Data part is empty but allocated
WavFile * wavInitPCM(uint32_t sNumber, uint32_t sRate, uint16_t depth, uint16_t ch) {
    
        WavFile * newFile;
        newFile = (WavFile *) malloc(sizeof(WavFile));

        //Fill structure
        uint8_t temp[4] = {'R', 'I', 'F', 'F'};
        memcpy(newFile->header.chunkId, temp, 4);
        newFile->header.chunkSize = 36 + sNumber * (depth / 8) * ch;
        temp[0] = 'W'; temp[1] = 'A';
        temp[2] = 'V'; temp[3] = 'E';
        memcpy(newFile->header.format, temp, 4);
        temp[0] = 'f'; temp[1] = 'm';
        temp[2] = 't'; temp[3] = ' ';
        memcpy(newFile->header.subchunk1Id, temp, 4);
        newFile->header.subchunk1Size = 16;
        newFile->header.audioFormat = 1; //PCM
        newFile->header.numChannels = ch;
        newFile->header.sampleRate = sRate;
        newFile->header.byteRate = sRate * (depth/8) * ch;
        newFile->header.blockAlign = (depth / 8) * ch;
        newFile->header.bitsPerSample = depth;
        temp[0] = 'd'; temp[1] = 'a';
        temp[2] = 't'; temp[3] = 'a';
        memcpy(newFile->header.subchunk2Id, temp, 4);
        newFile->header.subchunk2Size = sNumber * (depth / 8) * ch;

        newFile->data = (SampleType *) malloc(ch * sNumber * sizeof(SampleType));

        return newFile;
}

//Free memory allocated for wav-structure
int wavDestroy(WavFile * file) {
    if (file != NULL) {
        free(file->data);
        free(file);
        return 1;
    } else {
        return 0;
    }
}

//========================================================
//                    I/O Functions
//========================================================

// Read wav-file and write some information from header
// return array of samples if there isn't any error and NULL in other case
WavFile * wavInitFromFile(char * fileName) {

    input = freopen(fileName, "rb", stdin);
    if (input == NULL)
    {
        printf("Failed open file\n");
        return 0;
    }

    WavFile * file;
    file = (WavFile *) malloc(sizeof(WavFile));

    fread(&file->header, sizeof(WavHeader), 1, input);

    uint8_t waveCompare[4] = {'W', 'A', 'V', 'E'};
    if (memcmp(file->header.format, waveCompare, 4)) {
        return NULL;
    } else {

        // Duration in minutes and seconds
        uint32_t fDurationSeconds = file->header.subchunk2Size / (file->header.bitsPerSample / 8) / file->header.numChannels / file->header.sampleRate;
        uint32_t iDurationMinutes = fDurationSeconds / 60;
        fDurationSeconds = fDurationSeconds - (iDurationMinutes * 60);

        // Now we are ready to get samples from file
        file->data = (SampleType *) malloc(file->header.subchunk2Size / (file->header.bitsPerSample / 8) * sizeof(SampleType));

        // Read samples
        for (int i = 0; i < file->header.subchunk2Size; i++) {
            for (int j = 0; j < file->header.bitsPerSample / 8; j++) {
                fread(&file->data[i].bytes[j], sizeof(uint8_t), 1, input);
            }
        }

        fclose(input);

        return file;
    }
}

int wavWrite(WavFile * file, char * filepath) {

    // if we haven't received empty structure then let's write header and data to the file
    if (file != NULL) {
        output = freopen(filepath, "wb", stdout);

        // Header writing
        if (fwrite(&file->header, sizeof(WavHeader), 1, output) == 0) {
            printf("Failed write into a file\n");
            return 0;
        }

        // Data writing
        uint32_t bytesPerSample = file->header.bitsPerSample / 8;
        for (int i = 0; i < file->header.subchunk2Size; i++) {
            fwrite(&file->data[i/bytesPerSample].bytes[i%bytesPerSample], sizeof(uint8_t), 1, output);
        }

        fclose(output);

        return 1;

    } else {
        printf("Failed write into a file\n");
        return 0;
    }

}

// Write "raw" data into a file
// File structure: Number of bytes [4 bytes]
//                 Bits per sample [4 bytes]
//                 Data [...]
int rawWrite(SampleType * data, uint32_t n, uint16_t bitsPerSample) {

    if (data != NULL) {

        output = freopen("output.raw", "wb", stdout);

        fwrite(&n, sizeof(uint32_t), 1, output);
        fwrite(&bitsPerSample, sizeof(uint16_t), 1, output);

        uint32_t bytesPerSample = bitsPerSample / 8;
        for (uint32_t i = 0; i < n; i++) {
            fwrite(&data[i/bytesPerSample].bytes[i%bytesPerSample], sizeof(uint8_t), 1, output);
        }

        fclose(output);
        return 1;

    } else {
        printf("Failed write into a file\n");
        return 0;
    }

}

// Read data from "raw" file and put it into wav file
void readRaw(WavFile * fDest, char * fName) {
    
    input = freopen(fName, "rb", stdin);
    if (input == NULL)
    {
        printf("Failed open file\n");
        return;
    }

    fread(&fDest->header.subchunk2Size, sizeof(uint32_t), 1, input);
    fread(&fDest->header.bitsPerSample, sizeof(uint16_t), 1, input);

    // Read samples
    for (int i = 0; i < fDest->header.subchunk2Size; i++) {
        for (int j = 0; j < fDest->header.bitsPerSample / 8; j++) {
            fread(&fDest->data[i].bytes[j], sizeof(uint8_t), 1, input);
        }
    }

}



//========================================================
//                    Generators
//========================================================

int32_t sgn(double x) {
    return x >= 0 ? INT32_MAX : -(INT32_MAX);
}

double mSine(double index, double frequency) {
    return sin(index * frequency);
}

double mSaw(double index, double frequency) {
    return 2 * (index * frequency - floor(index * frequency )) - 1;
}

// TODO duration is double value
// Generator of simple waves (depth of sound is default = 32-bit)
WavFile * generateWave(uint32_t sRate, uint32_t sec, char * mode, uint32_t freq) {

    WavFile * genFile = wavInitPCM(sec * sRate, sRate, DEPTH, CHANNELS);

    if (strcmp(mode, "sin") == 0) {
        for (int i = 0; i < sRate*sec; i++) {
            genFile->data[i].dMode32 = (uint32_t) (INT32_MAX / 2) * mSine(i, 2*M_PI * freq / sRate);
        }
    } else if (strcmp(mode, "saw") == 0) {
        for (int i = 0; i < sRate*sec; i++) {
            genFile->data[i].dMode32 = (uint32_t) (INT32_MAX / 2) * mSaw(i, 2*M_PI * freq / sRate);
        }
    } else if (strcmp(mode, "sqr") == 0) {
        for (int i = 0; i < sRate*sec; i++) {
            genFile->data[i].dMode32 = sgn(mSine(i, 2*M_PI * freq / sRate)) / 2;
        }
    } else if (strcmp(mode, "tri") == 0) {
        for (int i = 0; i < sRate*sec; i++) {
            genFile->data[i].dMode32 = (uint32_t) (INT32_MAX / 2) * (2/M_PI) * asin(mSine(i, M_PI* freq / sRate));
        }
    }

    return genFile;
}

// Create new wave from several others
//  wave1     wave2             waveN
//  -----  +  -----  +  ...  +  -----
//    N         N                 N
// ATTENTION: headers of structures have to be equal (duration is minimal)
WavFile * mixer(int64_t N, ...) {
    va_list waves;
    va_start(waves, N);
    WavFile * firstWave = va_arg(waves, WavFile *);
    uint32_t size = firstWave->header.subchunk2Size / (firstWave->header.bitsPerSample / 8);
    WavFile * newWave = wavInitPCM(size , firstWave->header.sampleRate, DEPTH, CHANNELS);
    // Move data from first struture
    for (int i = 0; i < size; i++) {
        newWave->data[i].dMode32 = firstWave->data[i].dMode32 / N;
    }
    // Add other waves
    for (int i = 0; i < N-1; i++) {
        WavFile * currentWave = va_arg(waves, WavFile *);
        uint32_t currentSize = currentWave->header.subchunk2Size / (currentWave->header.bitsPerSample / 8);
        if (currentSize < size) {
            newWave->data = realloc(newWave->data, currentSize);
            size = currentSize;
        }
        // Plus plus plus plus...
        for (int j = 0; j < size; j++) {
            newWave->data[j].dMode32 += (currentWave->data[j].dMode32 / N);
        }
    }
    va_end(waves);
    return newWave;
}

// It works as mixer but we don't need allocate memory
// Result wave will be written into wave1
WavFile * mixer2(WavFile * wave1, WavFile * wave2) {
    uint32_t size = wave1->header.subchunk2Size / (wave1->header.bitsPerSample / 8);
    // Plus plus plus plus...
    for (int j = 0; j < size; j++) {
        wave1->data[j].dMode32 /= 2;
        wave1->data[j].dMode32 += (wave2->data[j].dMode32 / 2);
    }
}

//========================================================
//                    Filters
//========================================================

// simple linear ADSR volume envelope (mono)
// 0 <= a, d, s, r <= 100 (in percents)
// 0 <= susVal <= 1
void ADSR(WavFile * wave, double attack, double decay, double sustain, double susVal, double release) {
    //check arguments
    if (attack + decay + sustain + release != 100) { //mega cool condition :3
        return;
    }
    if (susVal < 0 || susVal > 1) {
        return;
    }

    int size = wave->header.subchunk2Size / (wave->header.bitsPerSample / 8);
    // build envelope wave
    //freopen("log.txt", "w", stdout);
    double envelopeWave[size];
    for (int i = 0; i < size; i++) {
        envelopeWave[i] = 1;
    }
    double attackLength = size / 100 * attack;
    for (int i = 0; i < attackLength; i++) { //attack zone
        envelopeWave[i] = (1 * i / attackLength);
        //printf("%lf\n", envelopeWave[i]);
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
        //printf("%lf\n", envelopeWave[i]);
    }
    
    // modulate amplitude of the input wave using the envelope wave
    for (int i = 0; i < size; i++) {
        wave->data[i].dMode32 = (int32_t) ((double) wave->data[i].dMode32 * envelopeWave[i]);
    }
}

void LFO(WavFile * wave, WavFile * modWave) {
    uint32_t size = wave->header.subchunk2Size / (wave->header.bitsPerSample / 8);
    for (int j = 0; j < size; j++) {
        wave->data[j].dMode32 = (int32_t) ((double) wave->data[j].dMode32 * modWave->data[j].dMode32 / (INT32_MAX / 2));
    }
}

#endif

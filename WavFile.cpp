//
// Created by charlie on 23.10.2020.
//

#include "WavFile.h"
#include <cstring>
#include "soundlib.h"

//TODO depth
WavFile::WavFile(uint32_t sNumber, uint32_t sRate, uint16_t depth, uint16_t channels) {
    initHeader();

    header.chunkSize = 36 + sNumber * (depth / 8) * channels;
    header.numChannels = channels;
    header.sampleRate = sRate;

    header.byteRate = sRate * (depth / 8) * channels;
    header.blockAlign = (depth / 8) * channels;
    header.bitsPerSample = depth;
    header.subchunk2Size = sNumber * (depth / 8) * channels;

    data = (DataType *) malloc(channels * sNumber * (header.bitsPerSample / 8));
}

//========================================================
//                    I/O Functions
//========================================================
// Read wav-file and write some information from header
// return array of samples if there isn't any error and NULL in other case
WavFile::WavFile(char *fileName) {
    initHeader();

    try {
        FILE *input;
        input = freopen(fileName, "rb", stdin);
        if (input == nullptr) {
            printf("Failed open file\n");
            throw "Can't open the file";
        }

        fread(&header, sizeof(WavHeader), 1, input);

        uint8_t waveCompare[4] = {'W', 'A', 'V', 'E'};
        if (memcmp(header.format, waveCompare, 4) != 0) {
            throw "That is not a wav file";
        } else {

            // Duration in minutes and seconds
            uint32_t fDurationSeconds =
                    header.subchunk2Size / (header.bitsPerSample / 8) / header.numChannels /
                    header.sampleRate;
            uint32_t iDurationMinutes = fDurationSeconds / 60;
            fDurationSeconds = fDurationSeconds - (iDurationMinutes * 60);

            // Now we are ready to get samples from file
            data = (DataType *) malloc(
                    header.subchunk2Size / (header.bitsPerSample / 8) * header.numChannels * sizeof(DataType));

            // Read samples
            size_t size = sizeof(DataType);
            for (int i = 0; i < header.subchunk2Size / size; i++) {
                //uint8_t bytesPerSample = header.bitsPerSample / 8;
                fread(&data[i], size, 1, input);
            }

            fclose(input);
        }
    } catch (char *exception) {
        printf("%s\n", exception);
    }
}

// Make WAV File from wave (only 1 channel)
WavFile::WavFile(Wave *wave) {
    initHeader();

    //Copy header
    uint32_t sRate = wave->getSampleRate();
    uint16_t depth = wave->getBitsPerSample();
    uint32_t sNumber = wave->getDataSize() / depth * 8;
    uint8_t channels = 1;

    header.chunkSize = 36 + sNumber * (depth / 8) * channels;
    header.numChannels = channels;
    header.sampleRate = sRate;

    header.byteRate = sRate * (depth / 8) * channels;
    header.blockAlign = (depth / 8) * channels;
    header.bitsPerSample = depth;
    header.subchunk2Size = sNumber * (depth / 8) * channels;

    data = (DataType *) malloc(channels * sNumber * (header.bitsPerSample / 8));

//Copy data
    memcpy(data, wave->getData(), header.subchunk2Size);
}

int WavFile::wavWrite(const char *filepath) {

    try {
        FILE *output;
        output = freopen(filepath, "wb", stdout);
        if (output == nullptr)
            throw "Can't write the file";

        // Header writing
        if (fwrite(&header, sizeof(WavHeader), 1, output) == 0) {
            printf("Failed write into a file\n");
            return 1;
        }

        // Data writing
        uint32_t bytesPerSample = header.bitsPerSample / 8;
        size_t size = sizeof(DataType);
        for (int i = 0; i < header.subchunk2Size / size; i++) {
            fwrite(&data[i], size, 1, output);
        }

        fclose(output);
    } catch (char *exception) {
        printf("%s\n", exception);
    }

    printf("%d\n", header.numChannels);
    return 0;
}

// Write "raw" data into a file
// File structure: Number of bytes [4 bytes]
//                 Bits per sample [4 bytes]
//                 Data [...]
int WavFile::rawWrite(char *filename) {

    if (data != nullptr) {
        FILE *output;
        output = freopen(filename, "wb", stdout);

        fwrite(&header.subchunk2Size, sizeof(uint32_t), 1, output);
        fwrite(&header.bitsPerSample, sizeof(uint16_t), 1, output);

        size_t size = sizeof(DataType);
        for (uint32_t i = 0; i < header.subchunk2Size / size; i++) {
            fwrite(&data[i], size, 1, output);
        }

        fclose(output);
        return 1;

    } else {
        printf("Failed write into a file\n");
        return 0;
    }

}

// TODO test
// Read data from "raw" file and put it into wav file
void WavFile::readRaw(char *fName) {

    FILE *input;
    input = freopen(fName, "rb", stdin);
    if (input == nullptr) {
        printf("Failed open file\n");
        return;
    }

    fread(&header.subchunk2Size, sizeof(uint32_t), 1, input);
    fread(&header.bitsPerSample, sizeof(uint16_t), 1, input);

    //TODO read according to bitsPerSample

    // Read samples
    size_t size = sizeof(DataType);
    for (int i = 0; i < header.subchunk2Size / size; i++) {
        fread(&data[i], size, 1, input);
    }

}

void WavFile::initHeader() {
    //INIT header
    uint8_t temp[4] = {'R', 'I', 'F', 'F'};
    memcpy(header.chunkId, temp, 4);
    temp[0] = 'W'; temp[1] = 'A';
    temp[2] = 'V'; temp[3] = 'E';
    memcpy(header.format, temp, 4);
    temp[0] = 'f'; temp[1] = 'm';
    temp[2] = 't'; temp[3] = ' ';
    memcpy(header.subchunk1Id, temp, 4);
    header.subchunk1Size = 16;
    header.audioFormat = 1; //PCM
    temp[0] = 'd'; temp[1] = 'a';
    temp[2] = 't'; temp[3] = 'a';
    memcpy(header.subchunk2Id, temp, 4);
}
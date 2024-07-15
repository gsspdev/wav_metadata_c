#ifndef WAV_READER_H
#define WAV_READER_H

#include <stdint.h>

// Define structures to hold WAV file header information

typedef struct {
    char ChunkID[4];        // "RIFF" in ASCII (0x52494646 big-endian form)
    uint32_t ChunkSize;     // 4-byte size of the rest of the chunk (file size - 8 bytes)
    char Format[4];         // "WAVE" in ASCII (0x57415645 big-endian form)
} RiffChunk;

typedef struct {
    RiffChunk riff;
    // "fmt " sub-chunk
    char SubChunk1ID[4];    // "fmt " in ASCII (0x666d7420 big-endian form)
    uint32_t SubChunk1Size; // 4-byte size of rest of "fmt " sub-chunk (16 for PCM)
    uint16_t AudioFormat;   // Audio format code, 1 for PCM (linear quantization)
    uint16_t NumChannels;   // No. of channels, 1 for mono, 2 for stereo, etc.
    uint32_t SampleRate;    // Sampling rate (blocks per second)
    uint32_t ByteRate;      // Byte rate: SampleRate * NumChannels * BitsPerSample / 8
    uint16_t BlockAlign;    // Data block size (bytes): NumChannels * BitsPerSample / 8
    uint16_t BitsPerSample; // Bits per sample
    // "data" sub-chunk
    char SubChunk2ID[4];    // Contains "data" in ASCII form (0x64617461 big-endian form)
    uint32_t SubChunk2Size; // Number of bytes in data: NumSamples * NumChannels * BitsPerSample/8
} WAVFILE;

// Function prototype for reading the WAV file header
int WAVFileOpen(const char *filename, WAVFILE *wavFile);

#endif // WAV_READER_H

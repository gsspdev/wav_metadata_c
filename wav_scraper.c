#include <stdio.h>
#include <stdint.h>
#include <errno.h>

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

int WAVFileOpen(const char *filename, WAVFILE *wavFile) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Failed to open file");
        return errno;
    }

    if (fread(wavFile, sizeof(WAVFILE), 1, file) != 1) {
        perror("Failed to read file");
        fclose(file);
        return errno;
    }

    fclose(file);
    return 0;
}

int main() {
    WAVFILE wavFile;
    int result = WAVFileOpen("audio/Kick.wav", &wavFile);
    if (result == 0) {
        printf("Successfully read WAV file header.\n");
        printf("ChunkID: %.4s\n", wavFile.riff.ChunkID);
        printf("ChunkSize: %u\n", wavFile.riff.ChunkSize);
        printf("Format: %.4s\n", wavFile.riff.Format);
        printf("AudioFormat: %u\n", wavFile.AudioFormat);
        printf("NumChannels: %u\n", wavFile.NumChannels);
        printf("SampleRate: %u\n", wavFile.SampleRate);
        printf("ByteRate: %u\n", wavFile.ByteRate);
        printf("BlockAlign: %u\n", wavFile.BlockAlign);
        printf("BitsPerSample: %u\n", wavFile.BitsPerSample);
        printf("SubChunk2ID: %.4s\n", wavFile.SubChunk2ID);
        printf("SubChunk2Size: %u\n", wavFile.SubChunk2Size);
    } else {
        printf("Failed to read WAV file header with error code: %d\n", result);
    }

    return 0;
}

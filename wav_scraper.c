#include <stdio.h>
#include <stdint.h>
#include <errno.h>

// Structure to hold the RIFF chunk
typedef struct {
    char ChunkID[4];        // "RIFF" in ASCII (0x52494646 big-endian form)
    uint32_t ChunkSize;     // 4-byte size of the rest of the chunk (file size - 8 bytes)
    char Format[4];         // "WAVE" in ASCII (0x57415645 big-endian form)
} RiffChunk;

// Structure to hold the fmt chunk
typedef struct {
    char SubChunk1ID[4];    // "fmt " in ASCII (0x666d7420 big-endian form)
    uint32_t SubChunk1Size; // 4-byte size of rest of "fmt " sub-chunk (16 for PCM)
    uint16_t AudioFormat;   // Audio format code, 1 for PCM (linear quantization)
    uint16_t NumChannels;   // No. of channels, 1 for mono, 2 for stereo, etc.
    uint32_t SampleRate;    // Sampling rate (blocks per second)
    uint32_t ByteRate;      // Byte rate: SampleRate * NumChannels * BitsPerSample / 8
    uint16_t BlockAlign;    // Data block size (bytes): NumChannels * BitsPerSample / 8
    uint16_t BitsPerSample; // Bits per sample
} FmtChunk;

// Structure to hold the data chunk
typedef struct {
    char SubChunk2ID[4];    // Contains "data" in ASCII form (0x64617461 big-endian form)
    uint32_t SubChunk2Size; // Number of bytes in data: NumSamples * NumChannels * BitsPerSample/8
} DataChunk;

// Structure to hold the entire WAV file header
typedef struct {
    RiffChunk riff;
    FmtChunk fmt;
    DataChunk data;
} WavFile;

int wav_file_open(const char *filename, WavFile *wav_file) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Failed to open file");
        return errno;
    }

    if (fread(&wav_file->riff, sizeof(RiffChunk), 1, file) != 1) {
        perror("Failed to read RIFF chunk");
        fclose(file);
        return errno;
    }

    if (fread(&wav_file->fmt, sizeof(FmtChunk), 1, file) != 1) {
        perror("Failed to read fmt chunk");
        fclose(file);
        return errno;
    }

    if (fread(&wav_file->data, sizeof(DataChunk), 1, file) != 1) {
        perror("Failed to read data chunk");
        fclose(file);
        return errno;
    }

    fclose(file);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <wav_file_path>\n", argv[0]);
        return 1;
    }

    WavFile wav_file;
    int result = wav_file_open(argv[1], &wav_file);
    if (result == 0) {
        printf("Successfully read WAV file header.\n");
        printf("ChunkID: %.4s\n", wav_file.riff.ChunkID);
        printf("ChunkSize: %u\n", wav_file.riff.ChunkSize);
        printf("Format: %.4s\n", wav_file.riff.Format);
        printf("AudioFormat: %u\n", wav_file.fmt.AudioFormat);
        printf("NumChannels: %u\n", wav_file.fmt.NumChannels);
        printf("SampleRate: %u\n", wav_file.fmt.SampleRate);
        printf("ByteRate: %u\n", wav_file.fmt.ByteRate);
        printf("BlockAlign: %u\n", wav_file.fmt.BlockAlign);
        printf("BitsPerSample: %u\n", wav_file.fmt.BitsPerSample);
        printf("SubChunk2ID: %.4s\n", wav_file.data.SubChunk2ID);
        printf("SubChunk2Size: %u\n", wav_file.data.SubChunk2Size);
    } else {
        printf("Failed to read WAV file header with error code: %d\n", result);
    }

    return 0;
}

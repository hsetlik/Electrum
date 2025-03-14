#include "AudioSystem.h"

static double _sampleRate = 44100.0;
static int _blockSize = 512;
static size_t _numChannels = 2;

namespace AudioSystem {
double getSampleRate() { return _sampleRate; }
int getBlockSize() { return _blockSize; }
size_t getNumChannels() { return _numChannels; }

void setSampleRate(double rate) { _sampleRate = rate; }
void setBlockSize(int size) { _blockSize = size; }
void setNumChannels(size_t num) { _numChannels = num; }
} // namespace AudioSystem

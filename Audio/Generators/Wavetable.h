#pragma once
#include "../../Parameters/MathUtil.h"
#include <array>
#define TABLE_SIZE 2048
#define WAVES_PER_TABLE 10
using Wave = std::array<float, TABLE_SIZE>;

namespace WaveUtil
{
    inline float valueAtPhase(float* wave, float phase)
    {
        return Math::flerp(wave[(int)std::floor(phase * (float)TABLE_SIZE)], wave[(int)std::ceil(phase * (float)TABLE_SIZE)], (phase * (float)TABLE_SIZE) - std::floor(phase * (float)TABLE_SIZE));
    }
}

using WavetableData = std::array<Wave, WAVES_PER_TABLE>;

struct BandLimitedWave
{
    float table[TABLE_SIZE];
    double minPhaseDelta;
    double maxPhaseDelta;
};

struct Wavetable
{
private:
    std::array<BandLimitedWave, WAVES_PER_TABLE> data;
    int tablesAdded;
    int bottomIndex;
    float bSample;
    float tSample;
    float skew;
    void createTables(int size, float* real, float* imag);
    float makeTable(float* waveReal, float* waveImag, int numSamples, float scale, float bottomFreq, float topFreq);
    BandLimitedWave* getWaveForHz(double hz, double sampleRate);
public:
    Wavetable (Wave& firstWave);
    float getSample(float phase, double freq, double sampleRate);
};

struct WavetableSet
{
private:
    std::vector<Wavetable> tables;
public:
    WavetableSet(std::vector<Wave>& waves);
    float getSample(float phase, float tablePos, double freq, double sampleRate);
};



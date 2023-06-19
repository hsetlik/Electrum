#include "Wavetable.h"




//================================================================================
Wavetable::Wavetable (Wave& firstWave)
{
    Wave fImag;
    Wave fReal;
    for(size_t i = 0; i < TABLE_SIZE; ++i)
    {
        fReal[i] = 0.0f;
        fImag[i] = firstWave[i];
    }
    WaveUtil::wavetableFFT(fReal.data(), fImag.data());
    createTables(fReal, fImag);
}


void Wavetable::createTables(Wave& real, Wave& imag)
{
    size_t size = (int)TABLE_SIZE;
    size_t idx;
    // zero DC offset and Nyquist (set first and last samples of each array to zero, in other words)
    real[0] = imag[0] = 0.0f;
    real[size >> 1] = imag[size >> 1] = 0.0f;
    int maxHarmonic = size >> 1;
    const double minVal = 0.000001f;
    while((fabs(real[(size_t)maxHarmonic]) + fabs(imag[(size_t)maxHarmonic]) < minVal) && maxHarmonic)
        --maxHarmonic;
    float topFreq = (float)(2.0f / 3.0f / maxHarmonic); //note:: topFreq is in units of phase fraction per sample, not Hz
    Wave ai;
    Wave ar;
    float scale = 0.0f;
    float lastMinFreq = 0.0f;
    size_t tables = 0;
    while(maxHarmonic)
    {
        // fill the table in with the needed harmonics
        for (idx = 0; idx < size; idx++)
            ar[idx] = ai[idx] = 0.0f;
        for (idx = 1; idx <= maxHarmonic; idx++)
        {
            ar[idx] = real[idx];
            ai[idx] = imag[idx];
            ar[size - idx] = real[size - idx];
            ai[size - idx] = imag[size - idx];
        }
        // make the wavetable
        scale = makeTable(ar, ai, scale, lastMinFreq, topFreq, tables);
        ++tables;
        lastMinFreq = topFreq;
        topFreq *= 2.0f;
        maxHarmonic >>= 1;
    }
}
float Wavetable::makeTable(Wave& waveReal, Wave& waveImag, float scale, float bottomFreq, float topFreq, size_t tablesAdded)
{
    data[tablesAdded].maxPhaseDelta = topFreq;
    data[tablesAdded].minPhaseDelta = bottomFreq;
    WaveUtil::wavetableFFT(waveReal.data(), waveImag.data());
    if (scale == 0.0f)
    {
        // get maximum value to scale to -1 - 1
        double max = 0.0f;
        for (size_t idx = 0; idx < TABLE_SIZE; idx++)
        {
            double temp = fabs(waveImag[idx]);
            if (max < temp)
                max = temp;
        }
        scale = 1.0f / max * 0.999f;
        //printf("Table: %d has scale: %f\n", tablesAdded, scale);
    }
    auto minLevel = std::numeric_limits<float>::max();
    auto maxLevel = std::numeric_limits<float>::min();
    for(size_t i = 0; i < TABLE_SIZE; ++i)
    {
        data[tablesAdded].table[i] = waveImag[i] * scale;
        if(data[tablesAdded].table[i] < minLevel)
            minLevel = data[tablesAdded].table[i];
        if(data[tablesAdded].table[i] > maxLevel)
            maxLevel = data[tablesAdded].table[i];
    }
    auto offset = maxLevel + minLevel;
    minLevel = std::numeric_limits<float>::max();
    maxLevel = std::numeric_limits<float>::min();
    for(size_t i = 0; i < TABLE_SIZE; ++i)
    {
        data[tablesAdded].table[i] -= (offset / 2.0f); //make sure each table has no DC offset
        if(data[tablesAdded].table[i] < minLevel)
            minLevel = data[tablesAdded].table[i];
        if(data[tablesAdded].table[i] > maxLevel)
            maxLevel = data[tablesAdded].table[i];
    }
    
    return (float)scale;
}
//================================================================================
BandLimitedWave* Wavetable::getWaveForHz(double hz, double sampleRate)
{
    static float phaseDelta = 0.0f;
    phaseDelta = (float)(hz / sampleRate);
    for (auto& wave : data)
    {
        if (wave.maxPhaseDelta > phaseDelta && wave.minPhaseDelta <= phaseDelta)
            return &wave;
    }
    return &data[WAVES_PER_TABLE - 1];
}

float Wavetable::getSample(float phase, double freq, double sampleRate)
{
    return WaveUtil::valueAtPhase(getWaveForHz(freq, sampleRate)->table, phase);
}
//=====================================================================================
WavetableSet::WavetableSet(std::vector<Wave> waves)
{
    for (size_t i = 0; i < waves.size(); ++i)
    {
        auto wave = waves[i];
        tables.push_back(Wavetable(wave));
    }
    
}

WavetableSet::WavetableSet(std::string waveData) : WavetableSet(WaveUtil::wavesFromString(waveData))
{

}

float WavetableSet::getSample(float phase, float tablePos, double freq, double sampleRate)
{
    static float lower;
    static float upper;
    auto fIdx = Math::fastFloor(tablePos * tables.size());
    lower = tables[fIdx].getSample(phase, freq, sampleRate);
    upper = tables[(fIdx + 1) % tables.size()].getSample(phase, freq, sampleRate);
    return Math::flerp(lower, upper, ((float)tablePos * tables.size()) - (float)Math::fastFloor(tablePos * tables.size()));
}



#pragma once
#include "../../Parameters/MathUtil.h"
#include <array>
#define TABLE_SIZE 2048
#define WAVES_PER_TABLE 10
using Wave = std::array<float, TABLE_SIZE>;

struct WaveUtil
{
    static float valueAtPhase(float* wave, float phase)
    {
        auto fIdx = Math::fastFloor(phase * (float)TABLE_SIZE);
        return Math::flerp(wave[fIdx], wave[(fIdx + 1) % TABLE_SIZE], (phase * (float)TABLE_SIZE) - (float)fIdx);
    }
    static float valueAtPhase(Wave& wave, float phase)
    {
        auto fIdx = Math::fastFloor(phase * (float)TABLE_SIZE);
        return Math::flerp(wave[fIdx], wave[(fIdx + 1) % TABLE_SIZE], (phase * (float)TABLE_SIZE) - (float)fIdx);
    }
    static float nonLerpedValueAtPhase(Wave& wave, float phase)
    {
        return wave[Math::fastFloor(phase * TABLE_SIZE)];
    }
    static Wave getRisingRampWave()
    {
        Wave wave;
        float delta = 2.0f / (float)TABLE_SIZE;
        for (size_t i = 0; i < TABLE_SIZE; i++)
        {
            wave[i] = -1.0f + (delta * (float)i);
        }
        return wave;
    }
    static Wave getFallingRampWave()
    {
        Wave wave;
        float delta = 2.0f / (float)TABLE_SIZE;
        for (size_t i = 0; i < TABLE_SIZE; i++)
        {
            wave[i] = 1.0f - (delta * (float)i);
        }
        return wave;
    }
    static Wave getPulseWave(float dutyCycle=0.5f)
    {
        Wave wave;
        for (size_t i = 0; i < TABLE_SIZE; i++)
        {
            if ((float)i / (float)TABLE_SIZE > dutyCycle)
                wave[i] = 1.0f;
            else
                wave[i] = -1.0f;
        }
        return wave;
    }
    static Wave getTriangleWave()
    {
        Wave wave;
        float delta = 4.0f / (float)TABLE_SIZE;
        float value = 1.0f;
        for (size_t i = 0; i < TABLE_SIZE; i++)
        {
            if (i <= TABLE_SIZE / 2)
            {
                value -= delta;
            }
            else
            {
                value += delta;
            }
            wave[i] = value;
        }
        return wave;
    }
    static std::vector<Wave> getDefaultWaveSet()
    {
        return 
        {
            getRisingRampWave(),
            getTriangleWave(),
            getPulseWave(0.1f),
            getPulseWave(0.2f),
            getPulseWave(0.3f),
            getPulseWave(0.4f),
            getPulseWave(0.5f),
            getPulseWave(0.6f),
            getFallingRampWave()
        };
    }

    static void wavetableFFT(float* ar, float* ai)
    {
        int N = (int)TABLE_SIZE;
        int i, j, k, L;            /* indexes */
        int M, TEMP, LE, LE1, ip;  /* M = log N */
        int NV2, NM1;
        double t;               /* temp */
        float Ur, Ui, Wr, Wi, Tr, Ti;
        float Ur_old;
        // if ((N > 1) && !(N & (N - 1)))   // make sure we have a power of 2
        NV2 = N >> 1;
        NM1 = N - 1;
        TEMP = N; /* get M = log N */
        M = 0;
        while (TEMP >>= 1)
            ++M;
        /* shuffle */
        j = 1;
        for (i = 1; i <= NM1; i++)
        {
            if(i<j)
            {             /* swap a[i] and a[j] */
                t = ar[j-1];
                ar[j-1] = ar[i-1];
                ar[i-1] = t;
                t = ai[j-1];
                ai[j-1] = ai[i-1];
                ai[i-1] = t;
            }
            k = NV2;             /* bit-reversed counter */
            while(k < j)
            {
                j -= k;
                k /= 2;
            }
            j += k;
        }
        LE = 1.0f;
        for (L = 1; L <= M; L++) {            // stage L
            LE1 = LE;                         // (LE1 = LE/2)
            LE *= 2;                          // (LE = 2^L)
            Ur = 1.0f;
            Ui = 0.0f;
            Wr = cos(M_PI/(float)LE1);
            Wi = -sin(M_PI/(float)LE1); // Cooley, Lewis, and Welch have "+" here
            for (j = 1; j <= LE1; j++)
            {
                for (i = j; i <= N; i += LE)
                { // butterfly
                    ip = i+LE1;
                    Tr = ar[ip-1] * Ur - ai[ip-1] * Ui;
                    Ti = ar[ip-1] * Ui + ai[ip-1] * Ur;
                    ar[ip-1] = ar[i-1] - Tr;
                    ai[ip-1] = ai[i-1] - Ti;
                    ar[i-1]  = ar[i-1] + Tr;
                    ai[i-1]  = ai[i-1] + Ti;
                }
                Ur_old = Ur;
                Ur = Ur_old * Wr - Ui * Wi;
                Ui = Ur_old * Wi + Ui * Wr;
            }
        }
    }

    static std::string toString(Wave& wave)
    {
        size_t waveSize = TABLE_SIZE * sizeof(float);
        char const * p = reinterpret_cast<char const *>(wave.data());
        std::string s (p, p + waveSize);
        return s;
    }

    static Wave fromString(const std::string& str)
    {
        const float* arr = reinterpret_cast<const float*>(str.c_str());
        Wave out;
        int len = (int)(str.size() / sizeof(float));
        jassert(len == (int)TABLE_SIZE);
        for (int i = 0; i < len; i++)
        {
            out[(size_t)i] = arr[i];
        }
        return out;
    }

    static std::vector<Wave> wavesFromString(const std::string& str)
    {
        const size_t waveLength = TABLE_SIZE * sizeof(float);
        std::string current = "";
        std::vector<Wave> out;
        for (auto& c : str)
        {
            current.push_back(c);
            if (current.size() == waveLength)
            {
                out.push_back(fromString(current));
                current = "";
            }
        }
        return out;
    }
    static std::string stringFromWaves(std::vector<Wave>& waves)
    {
        std::string out = "";
        for (auto& wave : waves)
        {
            out += toString(wave);
        }
        return out;
    }
};

struct BandLimitedWave
{
    Wave table;
    double minPhaseDelta;
    double maxPhaseDelta;
};

struct Wavetable
{
private:
    std::array<BandLimitedWave, WAVES_PER_TABLE> data;
    void createTables(Wave& real, Wave& imag);
    float makeTable(Wave& waveReal, Wave& waveImag, float scale, float bottomFreq, float topFreq, size_t tablesAdded);
    BandLimitedWave* getWaveForHz(double hz, double sampleRate);
public:
    Wavetable (Wave& firstWave);
    float getSample(float phase, double freq, double sampleRate);
    std::string getWaveAsString()
    {
        auto& w = data[0].table;
        return WaveUtil::toString(w);
    }
    Wave getBasicWave()
    {
        return data[0].table;
    }
};

struct WavetableSet
{
private:
    std::vector<Wavetable> tables;
public:
    WavetableSet(std::vector<Wave> waves);
    WavetableSet(std::string waveData);
    float getSample(float phase, float tablePos, double freq, double sampleRate);
    size_t numWaves() const { return tables.size(); }
    std::string getWavesAsString()
    {
        std::string out = "";
        for (auto& t : tables)
        {
            out += t.getWaveAsString();
        }
        return out;
    }
    std::vector<Wave> getBaseWaves()
    {
        std::vector<Wave> out;
        for(auto t : tables)
        {
            out.push_back(t.getBasicWave());
        }
        return out;
    }
};



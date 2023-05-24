#pragma once
#include "Identifiers.h"

class AudioSystem
{
private:
    AudioSystem();
    ~AudioSystem();
    //data
    double sampleRate;
    int blockSize;
    size_t numChannels;
    double getSampleRatePriv() const;
    int getBlockSizePriv() const;
    size_t getNumChannelsPriv() const;

    void setSampleRatePriv(double newRate) { sampleRate = newRate; }
    void setBlockSizePriv(int newSize) { blockSize = newSize; }
    void setNumChannelsPriv(size_t num) { numChannels = num; }
public:
    static double getSampleRate();
    static int getBlockSize();
    static size_t getNumChannels();

    static void setSampleRate(double rate);
    static void setBlockSize(int size);
    static void setNumChannels(size_t num);

JUCE_DECLARE_SINGLETON(AudioSystem, false)
};
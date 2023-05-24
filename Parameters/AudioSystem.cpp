#include "AudioSystem.h"

JUCE_IMPLEMENT_SINGLETON(AudioSystem);

AudioSystem::AudioSystem() :
sampleRate(44100.0f),
blockSize(512),
numChannels(2)
{

}

AudioSystem::~AudioSystem()
{
    clearSingletonInstance();
}

double AudioSystem::getSampleRatePriv() const 
{
    return sampleRate;
}
int AudioSystem::getBlockSizePriv() const 
{
    return blockSize;
}
size_t AudioSystem::getNumChannelsPriv() const 
{
    return numChannels;
}
//========================================================
double AudioSystem::getSampleRate() 
{
    auto i = getInstance();
    return i->getSampleRatePriv();
}
int AudioSystem::getBlockSize() 
{
    auto i = getInstance();
    return i->getBlockSizePriv();
}
size_t AudioSystem::getNumChannels() 
{
    auto i = getInstance();
    return i->getNumChannelsPriv();
}

void AudioSystem::setSampleRate(double rate)
{
    auto i = getInstance();
    i->setSampleRatePriv(rate);
}

void AudioSystem::setBlockSize(int size)
{
    auto i = getInstance();
    i->setBlockSizePriv(size);
}

void AudioSystem::setNumChannels(size_t num)
{
    auto i = getInstance();
    i->setNumChannelsPriv(num);
}
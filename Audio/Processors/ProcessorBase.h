#pragma once
#include "../../Parameters/MathUtil.h"

class ProcessorBase
{
  protected:
    const String processorName;
    const int voiceIndex;
  public:
    ProcessorBase(const String& name, int idx);
    virtual ~ProcessorBase();
    //the main entry point
    virtual float process(float value)=0;    
    // subclasses should use this for setup
    virtual void prepare(double sampleRate, size_t blockSize, size_t numChannels)=0;
};

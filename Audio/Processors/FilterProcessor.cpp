#include "FilterProcessor.h"

SynthFilter::SynthFilter(EVT* tree, int idx) :
ProcessorBase("SynthFilter", idx),
state(tree),
lastCutoff(CUTOFF_HZ_DEFAULT),
lastRes(0.0f),
lastFilterType(IDs::filterTypes[0])
{
  filter.reset(new SVFFilter());
}

void SynthFilter::prepare(double sRate, size_t blockSize, size_t numChannels) 
{
  dsp::ProcessSpec spec;
  spec.sampleRate = sRate;
  spec.maximumBlockSize = (uint32)blockSize;
  spec.numChannels = (uint32)numChannels;
  
  filter->prepare(spec);
}

float SynthFilter::process(float input)
{
  return filter->processSample(0, input);
}

float SynthFilter::process(float input, const String& name, float cutoff, float res)
{
  setFilterFor(name, cutoff, res);
  return process(input);
}

void SynthFilter::setFilterFor(const String& type, float cutoff, float resonance)
{
  bool typeChanged = false;
  if(type != lastFilterType)
  {
    lastFilterType = type;
    auto newType = typeFor(type);
    filter->setType(newType);
    typeChanged = true;
  }
  if(typeChanged || cutoff != lastCutoff || resonance != lastRes)
  {
    lastCutoff = cutoff;
    lastRes = resonance;
    filter->setCutoffFrequency(lastCutoff);
    filter->setResonance(lastRes);
  }
}



String SynthFilter::getFilterType(APVTS& state, float value)
{
  auto range = state.getParameterRange(IDs::filterType.toString()).getRange();
  auto fIndex = jmap(value, range.getStart(), range.getEnd(), 0.0f, (float)IDs::filterTypes.size());
  return IDs::filterTypes[(int)fIndex];

}

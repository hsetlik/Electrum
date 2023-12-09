#pragma once
#include "../../Parameters/ElectrumValueTree.h"
#include "ProcessorBase.h"

using SVFType = dsp::StateVariableTPTFilterType;
using SVFFilter = dsp::StateVariableTPTFilter<float>;

class SynthFilter : public ProcessorBase
{
private:
  static SVFType typeFor(const String &id)
  {
    if (id == IDs::filterTypes[0])
      return SVFType::lowpass;
    else
      return SVFType::highpass;
  }
  void setFilterFor(const String &type, float cutoff, float resonance);

public:
  EVT *const state;
  SynthFilter(EVT *tree, int idx);
  ~SynthFilter() override {}
  float process(float input) override;
  float process(float input, const String &type, float cutoff, float res);
  void processStereo(float &l, float &r, const String &type, float cutoff,
                     float res, float wetDry);
  void prepare(double sRate, size_t blockSize, size_t numChannels) override;

private:
  float lastCutoff;
  float lastRes;
  String lastFilterType;
  std::unique_ptr<SVFFilter> filter;
  std::unique_ptr<SVFFilter> lFilter;
  std::unique_ptr<SVFFilter> rFilter;
};

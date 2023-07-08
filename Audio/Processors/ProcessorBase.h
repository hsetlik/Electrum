#pragma once
#include "../../Parameters/MathUtil.h"

class ProcessorBase
{
  public:
    const String processorName;
    const int voiceIndex;
    ProcessorBase(const String& name, int idx);
};

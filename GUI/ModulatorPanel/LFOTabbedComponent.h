#pragma once
#include "LFOGraph.h"

class LFOTabbedComponent : public TabbedComponent
{
private:
  OwnedArray<LFOPanel> lfos;

public:
  LFOTabbedComponent(EVT *tree);
};

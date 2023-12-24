#pragma once
#include "LFOGraph.h"
#include "PerlinPanel.h"

class LFOTabbedComponent : public TabbedComponent
{
private:
  OwnedArray<LFOPanel> lfos;
  PerlinPanel perlin;

public:
  LFOTabbedComponent(EVT *tree);
};

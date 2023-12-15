#pragma once
#include "../../Parameters/ElectrumValueTree.h"
#include "../Color.h"

class EnvelopeLevelComponent : public Component, public Timer
{
public:
  EVT *const state;
  const int index;
  EnvelopeLevelComponent(EVT *tree, int idx);
  void paint(Graphics &g) override;
  void timerCallback() override { repaint(); }

private:
  Colour upper;
  Colour lower;
};

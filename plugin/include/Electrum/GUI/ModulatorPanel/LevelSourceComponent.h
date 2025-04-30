#pragma once
#include "Electrum/GUI/GUITypedefs.h"
#include "Electrum/Shared/ElectrumState.h"
#include "Electrum/Shared/GraphingData.h"
#include "../Modulation/ModSourceComponent.h"

class LevelSourceComponent : public Component,
                             public GraphingData::Listener,
                             public juce::AsyncUpdater {
private:
  ElectrumState* const state;
  BoundedAttString monoLabel;
  ModSourceComponent monoSrc;
  BoundedAttString polyLabel;
  ModSourceComponent polySrc;
  float lastPolyLevel = 0.0f;
  float lastMonoLevel = 0.0f;

public:
  LevelSourceComponent(ElectrumState* s);
  ~LevelSourceComponent() override;
  void graphingDataUpdated(GraphingData* gd) override;
  void handleAsyncUpdate() override { repaint(); }
  void resized() override;
  void paint(juce::Graphics& g) override;
};


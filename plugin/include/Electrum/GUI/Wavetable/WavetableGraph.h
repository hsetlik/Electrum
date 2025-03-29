#pragma once

#include <stack>
#include "Electrum/Audio/Wavetable.h"
#include "Electrum/GUI/LookAndFeel/Color.h"
#include "Electrum/Shared/GraphingData.h"
#include "Electrum/Shared/ElectrumState.h"

#include "juce_opengl/juce_opengl.h"

//==================================================================
typedef juce::Vector3D<float> vec3D_f;

#define GRAPH_REFRESH_HZ 24
#define GRAPH_W 512
#define GRAPH_H 512

#define Z_SETBACK 0.6f
#define CAMERA_DISTANCE 0.3f

struct wave_path_t {
  juce::Path path;
  color_t color;
  float zPosition;
  float strokeWeight;
};

class WavetableGraph : public GraphingData::Listener,
                       public juce::Component,
                       public juce::Timer {
private:
  ElectrumState* const state;
  juce::Image img;
  bool wantsWaveData = true;
  bool redrawRequested = false;
  // position state stuff
  float lastDrawnPos = 0.0f;
  float currentPos = 0.0f;
  // and the sets of paths we need to draw
  std::stack<wave_path_t> wavePaths;

public:
  const int oscID;
  WavetableGraph(ElectrumState* s, int idx);
  ~WavetableGraph() override;
  bool needsWaveData() const { return wantsWaveData; }
  void graphingDataUpdated(GraphingData* gd) override;
  void timerCallback() override;
  // this needs to be called at startup and
  // then whenever a new wave is selected
  void loadWaveDataFrom(Wavetable* wt);

private:
  void updateGraphImage();
};

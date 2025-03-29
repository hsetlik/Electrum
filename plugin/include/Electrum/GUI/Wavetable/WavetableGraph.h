#pragma once

#include <stack>
#include "Electrum/Audio/Wavetable.h"
#include "Electrum/GUI/LookAndFeel/Color.h"
#include "Electrum/Shared/GraphingData.h"
#include "Electrum/Shared/ElectrumState.h"

#include "juce_core/system/juce_PlatformDefs.h"
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
  float zPosition;
  float strokeWeight;
};

class WavetableGraph : public GraphingData::Listener,
                       public juce::Component,
                       public juce::Timer {
private:
  ElectrumState* const state;
  juce::Image img;
  bool redrawRequested = false;
  // position state stuff
  float lastDrawnPos = 0.0f;
  float currentPos = 0.0f;
  // and the sets of paths we need to draw
  std::vector<wave_path_t> wavePaths = {};

public:
  const int oscID;
  WavetableGraph(ElectrumState* s, int idx);
  void graphingDataUpdated(GraphingData* gd) override;
  void wavePointsUpdated(GraphingData* gd, int id) override;
  void timerCallback() override;
  void paint(juce::Graphics& g) override;

private:
  void updateWavePaths(GraphingData* gd);
  void updateGraphImage();
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WavetableGraph)
};

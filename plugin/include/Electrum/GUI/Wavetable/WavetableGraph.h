#pragma once

#include <stack>
#include "Electrum/Audio/Wavetable.h"
#include "Electrum/GUI/LookAndFeel/Color.h"
#include "Electrum/Shared/GraphingData.h"
#include "Electrum/Shared/ElectrumState.h"

#include "juce_core/juce_core.h"
#include "juce_core/system/juce_PlatformDefs.h"
#include "juce_events/juce_events.h"
#include "juce_opengl/juce_opengl.h"

//==================================================================
typedef juce::Vector3D<float> vec3D_f;
typedef juce::Point<float> fpoint_t;

#define GRAPH_REFRESH_HZ 24
#define GRAPH_W 512
#define GRAPH_H 512

#define Z_SETBACK 0.6f
#define CAMERA_DISTANCE 0.3f

// juce's builtin path class has done me wrong here
constexpr size_t WAVE_PATH_VERTS = WAVE_GRAPH_POINTS + 2;
class WavePath2D {
private:
  fpoint_t points[WAVE_PATH_VERTS] = {};

public:
  WavePath2D(const WavePath2D& other);
  WavePath2D() = default;
  fpoint_t& operator[](size_t idx) {
    jassert(idx < WAVE_PATH_VERTS);
    return points[idx];
  }
  fpoint_t operator[](size_t idx) const {
    jassert(idx < WAVE_PATH_VERTS);
    return points[idx];
  }
  void draw(juce::Graphics& g, float strokeWeight);
};

struct wave_path_t {
  WavePath2D path;
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

  void drawWaveGraph();

public:
  const int oscID;
  WavetableGraph(ElectrumState* s, int idx);
  void graphingDataUpdated(GraphingData* gd) override;
  void wavePointsUpdated(GraphingData* gd, int id) override;
  void paint(juce::Graphics& g) override;
  void timerCallback() override;

private:
  void updateWavePaths(GraphingData* gd);
  void updateGraphImage();
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WavetableGraph)
};

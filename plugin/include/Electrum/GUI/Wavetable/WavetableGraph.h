#pragma once

#include "Electrum/Shared/GraphingData.h"
#include "Electrum/Shared/ElectrumState.h"

#include "Mat3x3.h"
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

typedef std::array<vec3D_f, WAVE_PATH_VERTS> vertex_arr_t;
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
  Mat3x3 matForward;
  Mat3x3 matInverse;
  // and the sets of paths we need to draw
  juce::OwnedArray<wave_path_t, juce::CriticalSection> wavePaths;

  void drawWaveGraph();

  bool firstPathsGenerated = false;

public:
  const int oscID;
  WavetableGraph(ElectrumState* s, int idx);
  void graphingDataUpdated(GraphingData* gd) override;
  void wavePointsUpdated(GraphingData* gd, int id) override;
  void paint(juce::Graphics& g) override;
  void timerCallback() override;
  static Mat3x3 graphRotationMatrix() {
    const float xAngle = juce::MathConstants<float>::pi * 1.0f;
    const float yAngle = juce::MathConstants<float>::pi * -0.6f;
    const float zAngle = juce::MathConstants<float>::pi * -0.35f;
    return Mat3x3::getRotationMatrix(xAngle, yAngle, zAngle);
  }

private:
  // just so we don't reallocate these every time
  single_wave_norm_t waveNorm;
  vertex_arr_t vertArray;

  void updateWavePaths(GraphingData* gd);
  void updateGraphImage();

  void addPathFor(const vertex_arr_t& vertices);

  fpoint_t projectToCanvas(vec3D_f point);
  vec3D_f vertexFromCanvas(fpoint_t f, float zPos);
  std::vector<vec3D_f> getVerticesForWave(const single_wave_norm_t& normPoints,
                                          float zPos);
  void loadVerticesForWave(vertex_arr_t& dest,
                           const single_wave_norm_t& normPoints,
                           float zPos);
  wave_path_t getVirtualWave(float normPos);
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WavetableGraph)
};

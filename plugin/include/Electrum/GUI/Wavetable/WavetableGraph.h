#pragma once

#include "Electrum/Identifiers.h"
#include "Electrum/Shared/GraphingData.h"
#include "Electrum/Shared/ElectrumState.h"

#include "Mat3x3.h"
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

#define WAVE_PATH_POINTS (WAVE_GRAPH_POINTS + 2)

typedef std::array<vec3D_f, WAVE_PATH_POINTS> wave_vertices_t;
typedef std::array<fpoint_t, WAVE_PATH_POINTS> wave_points_t;

class WavetableGraph : public Component, public juce::Timer {
private:
  ElectrumState* const state;
  juce::Image img;
  float currentWavePos = 0.002f;

  std::vector<wave_vertices_t> waveVertArrays = {};
  wave_vertices_t virtualVerts;

  Mat3x3 rotationMatrix;
  bool wavesReady = false;

public:
  const int oscID;
  WavetableGraph(ElectrumState* s, int idx);
  void timerCallback() override;
  void paint(juce::Graphics& g) override;

private:
  void updateVertices(const String& wavetableStr);
  void projectToPoints(const wave_vertices_t& verts, wave_points_t& points);
  void updateVirtualVertices();
  void redrawBitmap();
  void drawWave(juce::Graphics& g, const wave_points_t& points, float stroke);
};

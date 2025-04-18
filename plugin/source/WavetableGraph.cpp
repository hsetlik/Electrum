#include "Electrum/GUI/Wavetable/WavetableGraph.h"
#include <cmath>
#include "Electrum/Audio/AudioUtil.h"
#include "Electrum/Audio/Wavetable.h"
#include "Electrum/GUI/LookAndFeel/Color.h"
#include "Electrum/GUI/Wavetable/Mat3x3.h"
#include "Electrum/Identifiers.h"
#include "Electrum/Shared/GraphingData.h"
#include "juce_graphics/juce_graphics.h"
// Wave Path drawing================================

static void loadVertsForWave(wave_vertices_t& dest,
                             float* waveData,
                             float zPos) {
  dest[0] = {0.0f, 0.0f, zPos};
  for (size_t i = 0; i < WAVE_GRAPH_POINTS; ++i) {
    const float xPos = (float)i / (float)WAVE_GRAPH_POINTS;
    const size_t wIdx = (size_t)(xPos * (float)TABLE_SIZE);
    const float yPos = (waveData[wIdx] + 1.0f) / 2.0f;
    dest[i + 1] = {xPos, yPos, zPos};
  }
  dest[WAVE_PATH_POINTS - 1] = {1.0f, 0.0f, zPos};
}

void WavetableGraph::updateVertices(const String& waveStr) {
  waveVertArrays.clear();
  auto waves = splitWaveStrings(waveStr);
  float waveBuf[TABLE_SIZE];
  float zPos;
  for (int i = 0; i < waves.size(); ++i) {
    zPos = ((float)i / (float)waves.size()) + Z_SETBACK;
    // 1. decode string into waveBuf
    stringDecodeWave(waves[i], waveBuf);
    // 2. convert into vertices
    wave_vertices_t verts;
    loadVertsForWave(verts, waveBuf, zPos);
    waveVertArrays.push_back(verts);
  }
  jassert(waveVertArrays.size() > 1);
}

void WavetableGraph::updateVirtualVertices() {
  // 1. find our z position
  const float zPos = currentWavePos + Z_SETBACK;
  // 2. figure out which two waves we're between and in what proportion
  const size_t lowIdx = AudioUtil::fastFloor64(
      currentWavePos * (float)(waveVertArrays.size() - 1));
  const size_t highIdx = lowIdx + 1;
  jassert(lowIdx != highIdx);
  const float t = (zPos - waveVertArrays[lowIdx][0].z) /
                  (waveVertArrays[highIdx][0].z - waveVertArrays[lowIdx][0].z);
  // 3. calculate each vertex
  virtualVerts[0] = {0.0f, 0.0f, zPos};
  float xPos, yPos;
  for (size_t i = 1; i <= WAVE_GRAPH_POINTS; ++i) {
    xPos = (float)i / (float)WAVE_GRAPH_POINTS;
    yPos = flerp(waveVertArrays[lowIdx][i].y, waveVertArrays[highIdx][i].y, t);
    virtualVerts[i] = {xPos, yPos, zPos};
  }
  virtualVerts[WAVE_PATH_POINTS - 1] = {1.0f, 0.0f, zPos};
}

// 3D math stuff---------------

static void projectToPoints(const wave_vertices_t& verts,
                            wave_points_t& points,
                            Mat3x3 rotationMatrix) {
  const float yHeight = 1.4f;
  vec3D_f c = {-0.5f, yHeight, 0.0f};         // represents the camera pinhole
  vec3D_f e = {0.0f, 0.0f, CAMERA_DISTANCE};  // represents
  for (size_t i = 0; i < WAVE_PATH_POINTS; ++i) {
    auto d = verts[i] - c;
    // multiply by the rotation matrix
    d = rotationMatrix * d;
    // now convert to the 2d plane
    float xPos = ((e.z / d.z) * d.x) + e.x;
    float yPos = ((e.z / d.z) * d.y) + e.y;
    points[i] = {xPos * (float)GRAPH_W, yPos * (float)GRAPH_H};
  }
}

static color_t _getWaveColor(float waveNorm, float virtualNorm) {
  static color_t nColor = Color::qualifierPurple.darker(0.5f);
  static color_t vColor = Color::qualifierPurple.brighter(0.3f);
  static frange_t tRange = rangeWithCenter(0.0f, 1.0f, 0.2f);
  const float tLin = std::fabs(waveNorm - virtualNorm);
  const float t = tRange.convertFrom0to1(tLin);
  return nColor.interpolatedWith(vColor, t);
}

static float _getStrokeWidth(float waveZ) {
  static frange_t strokeRange = rangeWithCenter(1.8f, 3.8f, 3.3f);
  return strokeRange.convertFrom0to1(1.0f - (waveZ - Z_SETBACK));
}

static void drawWave(juce::Graphics& g,
                     const wave_points_t& points,
                     float stroke) {
  // 1. convert to juce::Path object
  juce::Path path;
  path.startNewSubPath(points[0]);
  for (size_t i = 1; i < WAVE_PATH_POINTS; ++i) {
    path.lineTo(points[i]);
  }
  // path.closeSubPath();
  //  2. draw it
  juce::PathStrokeType pst(stroke);
  g.strokePath(path, pst);
}

void WavetableGraph::redrawBitmap() {
  img.clear(img.getBounds());
  juce::Graphics g(img);
  wave_points_t pathPoints;
  const size_t virtualIdx = AudioUtil::fastFloor64(
      currentWavePos * (float)(waveVertArrays.size() - 1));
  for (size_t i = 0; i < waveVertArrays.size(); ++i) {
    // 1. project to 2d points
    float zPos = waveVertArrays[i][0].z;
    projectToPoints(waveVertArrays[i], pathPoints, rotationMatrix);
    // 2. set color and stroke width
    auto color = _getWaveColor(zPos - Z_SETBACK, currentWavePos);
    auto stroke = _getStrokeWidth(zPos);
    g.setColour(color);
    // 3. draw the wave
    drawWave(g, pathPoints, stroke);
    // 4. check if it's time to draw the virtual wave
    if (i == virtualIdx) {
      zPos = virtualVerts[0].z;
      projectToPoints(virtualVerts, pathPoints, rotationMatrix);
      g.setColour(Color::qualifierPurple);
      stroke = _getStrokeWidth(zPos) * 1.035f;
      drawWave(g, pathPoints, stroke);
    }
  }
}
//===================================================

WavetableGraph::WavetableGraph(ElectrumState* s, int osc)
    : state(s), img(juce::Image::RGB, GRAPH_W, GRAPH_H, true), oscID(osc) {
  // 1. initialize our vertices
  if (!state->graph.needsWavetableData()) {
    auto str = state->graph.getWavetableString(oscID);
    updateVertices(str);
    updateVirtualVertices();
    wavesReady = true;
  }
  // 2. calculate the rotation matrix
  const float xAngle = juce::MathConstants<float>::pi * 1.0f;
  const float yAngle = juce::MathConstants<float>::pi * -0.6f;
  const float zAngle = juce::MathConstants<float>::pi * -0.35f;
  rotationMatrix = Mat3x3::getRotationMatrix(xAngle, yAngle, zAngle);
  // 3. attach the GraphingData::Listener
  state->graph.addListener(this);
  // 4. start the timer
  startTimerHz(GRAPH_REFRESH_HZ);
}

WavetableGraph::~WavetableGraph() {
  state->graph.removeListener(this);
}

void WavetableGraph::timerCallback() {
  if (wavesReady) {
    auto _pos = state->graph.getOscPos(oscID);
    if (!fequal(_pos, currentWavePos)) {
      currentWavePos = _pos;
      updateVirtualVertices();
    }
    redrawBitmap();
  } else if (!state->graph.needsWavetableData()) {
    auto str = state->graph.getWavetableString(oscID);
    updateVertices(str);
    updateVirtualVertices();
    redrawBitmap();
    wavesReady = true;
  }
  repaint();
}

void WavetableGraph::paint(juce::Graphics& g) {
  auto fBounds = getLocalBounds().toFloat();
  g.drawImage(img, fBounds);
}

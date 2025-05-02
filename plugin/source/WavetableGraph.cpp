#include "Electrum/GUI/Wavetable/WavetableGraph.h"
#include <cmath>
#include "Electrum/Audio/AudioUtil.h"
#include "Electrum/Audio/Wavetable.h"
#include "Electrum/GUI/GUITypedefs.h"
#include "Electrum/GUI/LookAndFeel/Color.h"
#include "Electrum/GUI/Wavetable/Mat3x3.h"
#include "Electrum/Identifiers.h"
#include "Electrum/Shared/GraphingData.h"
#include "juce_graphics/juce_graphics.h"
// Wave Path drawing================================

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

static float minYPosition(const wave_vertices_t& verts) {
  float min = 10000000.0f;
  for (auto& v : verts) {
    if (v.y < min) {
      min = v.y;
    }
  }
  return min;
}

vec3D_f WavetableGraph::vertexLerp(const vec3D_f& a,
                                   const vec3D_f& b,
                                   float t) {
  const float x = flerp(a.x, b.x, t);
  const float y = flerp(a.y, b.y, t);
  const float z = flerp(a.z, b.z, t);
  return {x, y, z};
}

void WavetableGraph::updateVertices(const String& waveStr) {
  waveVertArrays.clear();
  wavePaths.clear();
  auto waves = splitWaveStrings(waveStr);
  float waveBuf[TABLE_SIZE];
  float zPos;
  minVertexY = 1000.0f;
  for (int i = 0; i < waves.size(); ++i) {
    zPos = ((float)i / (float)waves.size()) + Z_SETBACK;
    // 1. decode string into waveBuf
    stringDecodeWave(waves[i], waveBuf);
    // 2. convert into vertices
    wave_vertices_t verts;
    loadVertsForWave(verts, waveBuf, zPos);
    waveVertArrays.push_back(verts);
    // 3. check the minimum y vertex
    float lowestY = minYPosition(verts);
    if (lowestY < minVertexY) {
      minVertexY = lowestY;
    }
    // 4. convert to a path
    wave_points_t points;
    projectToPoints(verts, points, rotationMatrix);
    juce::Path path;
    path.startNewSubPath(points[0]);
    for (size_t p = 1; p < WAVE_PATH_POINTS; ++p) {
      path.lineTo(points[p]);
    }
    wavePaths.push_back(path);
  }
  jassert(waveVertArrays.size() > 1);
}

size_t WavetableGraph::waveIndexBelow(float wavePos) const {
  for (size_t i = 1; i < waveVertArrays.size(); ++i) {
    const float prevPos = waveVertArrays[i - 1][0].z - Z_SETBACK;
    const float currentPos = waveVertArrays[i][0].z - Z_SETBACK;
    if (prevPos <= wavePos && currentPos > wavePos) {
      return i - 1;
    }
  }
  jassert(false);
  return 0;
}

void WavetableGraph::updateVirtualVertices() {
  // 1. find our z position
  const float maxZPos = waveVertArrays.back()[0].z - 0.00021f;
  const float zPos = std::min(currentWavePos + Z_SETBACK, maxZPos);
  // 2. figure out which two waves we're between and in what proportion
  const size_t lowIdx = waveIndexBelow(zPos - Z_SETBACK);
  const size_t highIdx = lowIdx + 1;
  // jassert(highIdx > lowIdx);
  // jassert(highIdx > 0);
  const float lowZ = waveVertArrays[lowIdx][0].z;
  const float highZ = waveVertArrays[highIdx][0].z;
  const float t = (zPos - lowZ) / (highZ - lowZ);
  jassert(t <= 1.0f);
  jassert(t >= 0.0f);
  // 3. calculate each vertex
  virtualVerts[0] = {0.0f, 0.0f, zPos};
  for (size_t i = 0; i < WAVE_GRAPH_POINTS; ++i) {
    auto& vLow = waveVertArrays[lowIdx][i];
    auto& vHigh = waveVertArrays[highIdx][i];
    virtualVerts[i + 1] = vertexLerp(vLow, vHigh, t);
  }
  virtualVerts[WAVE_PATH_POINTS - 1] = {1.0f, 0.0f, zPos};
  // 4. convert it to a path
  wave_points_t vPoints;
  projectToPoints(virtualVerts, vPoints, rotationMatrix);
  virtualWavePath.clear();
  virtualWavePath.startNewSubPath(vPoints[0]);
  for (size_t i = 1; i < WAVE_PATH_POINTS; ++i) {
    virtualWavePath.lineTo(vPoints[i]);
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

// Bitmap redrawing code===========================
void WavetableGraph::redrawBitmap() {
  if (singleWaveMode) {
    redrawBitmapSingle(idleImg);
  } else {
    redrawBitmapMulti(idleImg);
  }
}

void WavetableGraph::redrawBitmapMulti() {
  imgA.clear(imgA.getBounds(), Color::nearBlack);
  juce::Graphics g(imgA);
  wave_points_t pathPoints;
  wave_points_t virtualPoints;
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
    juce::PathStrokeType pst(stroke);
    g.strokePath(wavePaths[i], pst);
    // 4. check if it's time to draw the virtual wave
    if (i == virtualIdx) {
      zPos = virtualVerts[0].z;
      projectToPoints(virtualVerts, virtualPoints, rotationMatrix);
      g.setColour(Color::qualifierPurple);
      stroke = _getStrokeWidth(zPos) * 1.035f;
      juce::PathStrokeType vPst(stroke);
      g.strokePath(virtualWavePath, vPst);
    }
  }
}

void WavetableGraph::redrawBitmapMulti(juce::Image* img) {
  img->clear(img->getBounds(), Color::nearBlack);
  juce::Graphics g(*img);
  wave_points_t pathPoints;
  wave_points_t virtualPoints;
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
    juce::PathStrokeType pst(stroke);
    g.strokePath(wavePaths[i], pst);
    // 4. check if it's time to draw the virtual wave
    if (i == virtualIdx) {
      zPos = virtualVerts[0].z;
      projectToPoints(virtualVerts, virtualPoints, rotationMatrix);
      g.setColour(Color::qualifierPurple);
      stroke = _getStrokeWidth(zPos) * 1.035f;
      juce::PathStrokeType vPst(stroke);
      g.strokePath(virtualWavePath, vPst);
    }
  }
}
void WavetableGraph::redrawBitmapSingle() {
  imgA.clear(imgA.getBounds(), Color::nearBlack);
  juce::Graphics g(imgA);
  auto fBounds = imgA.getBounds().toFloat().reduced(8.0f);
  const float x0 = fBounds.getX();
  juce::Path path;
  path.startNewSubPath(x0, fBounds.getBottom());
  float x, y;
  for (size_t i = 1; i < WAVE_PATH_POINTS; ++i) {
    x = x0 + (virtualVerts[i].x * fBounds.getWidth());
    y = fBounds.getBottom() - (virtualVerts[i].y * fBounds.getHeight());
    path.lineTo(x, y);
  }
  // path.closeSubPath();
  g.setColour(Color::qualifierPurple);
  juce::PathStrokeType pst(2.5f);
  g.strokePath(path, pst);
}

void WavetableGraph::redrawBitmapSingle(juce::Image* img) {
  img->clear(img->getBounds(), Color::nearBlack);
  juce::Graphics g(*img);
  auto fBounds = img->getBounds().toFloat().reduced(8.0f);
  const float x0 = fBounds.getX();
  juce::Path path;
  path.startNewSubPath(x0, fBounds.getBottom());
  float x, y;
  for (size_t i = 1; i < WAVE_PATH_POINTS; ++i) {
    x = x0 + (virtualVerts[i].x * fBounds.getWidth());
    y = fBounds.getBottom() - (virtualVerts[i].y * fBounds.getHeight());
    path.lineTo(x, y);
  }
  // path.closeSubPath();
  g.setColour(Color::qualifierPurple);
  juce::PathStrokeType pst(2.5f);
  g.strokePath(path, pst);
}

//===================================================

WavetableGraph::WavetableGraph(ElectrumState* s, int osc)
    : state(s),
      imgA(juce::Image::RGB, GRAPH_W, GRAPH_H, true),
      imgB(juce::Image::RGB, GRAPH_W, GRAPH_H, true),
      oscID(osc) {
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

void WavetableGraph::mouseUp(const juce::MouseEvent& me) {
  if (me.mouseWasClicked()) {
    singleWaveMode = !singleWaveMode;
    triggerAsyncUpdate();
  }
}

void WavetableGraph::enablementChanged() {
  if (isEnabled()) {
    if (!isTimerRunning())
      startTimerHz(GRAPH_REFRESH_HZ);
  } else {
    stopTimer();
  }
}

void WavetableGraph::handleAsyncUpdate() {
  // 1. compute the vertices/paths
  if (!wavesReady) {
    auto str = state->graph.getWavetableString(oscID);
    updateVertices(str);
    updateVirtualVertices();
    wavesReady = true;
  } else {
    updateVirtualVertices();
  }
  // 2. redraw the image
  redrawBitmap();
  // 3. swap the image pointers
  auto* prevViewed = viewedImg;
  viewedImg = idleImg;
  idleImg = prevViewed;
  // 4. repaint
  repaint();
}

void WavetableGraph::timerCallback() {
  if (wavesReady) {
    auto _pos = state->graph.getOscPos(oscID);
    if (!fequal(_pos, currentWavePos)) {
      currentWavePos = _pos;
      triggerAsyncUpdate();
    }
  } else if (!state->graph.needsWavetableData()) {
    triggerAsyncUpdate();
  }
}

void WavetableGraph::paint(juce::Graphics& g) {
  auto fBounds = getLocalBounds().toFloat();
  g.drawImage(*viewedImg, fBounds);
}

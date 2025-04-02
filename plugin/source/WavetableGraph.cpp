#include "Electrum/GUI/Wavetable/WavetableGraph.h"
#include <cmath>
#include "Electrum/GUI/LookAndFeel/Color.h"
// Wave Path drawing================================

void WavePath2D::draw(juce::Graphics& g, float strokeWeight) {
  juce::PathStrokeType pst(strokeWeight);
  // render our points as a juce path
  juce::Path jPath;
  jPath.startNewSubPath(points[0].getX(), points[0].getY());
  for (size_t i = 1; i < WAVE_PATH_VERTS; ++i) {
    jPath.lineTo(points[i].getX(), points[i].getY());
  }
  jPath.closeSubPath();
  // do the drawing
  g.strokePath(jPath, pst);
}

WavePath2D::WavePath2D(const WavePath2D& other) {
  for (size_t i = 0; i < WAVE_PATH_VERTS; ++i) {
    points[i] = other[i];
  }
}

//===================================================

fpoint_t WavetableGraph::projectToCanvas(vec3D_f point3d) {
  constexpr float yHeight = 1.4f;
  vec3D_f cameraPos = {-0.5f, yHeight, 0.0f};
  // represents the display surface
  vec3D_f dSurface = {0.0f, 0.0f, CAMERA_DISTANCE};

  // adjust by the camera position
  auto pt = point3d - cameraPos;
  // apply the rotation
  pt = matForward * pt;
  // back to 2d
  const float fX = ((dSurface.z / pt.z) * pt.x) + dSurface.x;
  const float fY = ((dSurface.z / pt.z) * pt.y) + dSurface.y;
  jassert(fX >= 0.0f && fX <= 1.0f);
  jassert(fY >= 0.0f && fY <= 1.0f);
  return {fX * (float)GRAPH_W, fY * (float)GRAPH_H};
}

// inverse of the above
vec3D_f WavetableGraph::vertexFromCanvas(juce::Point<float> f, float zPos) {
  constexpr float yHeight = 1.4f;
  f.x /= (float)GRAPH_W;
  f.y /= (float)GRAPH_H;
  vec3D_f cameraPos = {-0.5f, yHeight, 0.0f};
  // represents the display surface
  vec3D_f dSurface = {0.0f, 0.0f, CAMERA_DISTANCE};

  // fX = ((dZ / pZ) * pX) + dX
  // pX = (fX - dX) / (dZ / pZ);
  const float pX = (f.x - dSurface.x) / (dSurface.z / zPos);
  const float pY = (f.y - dSurface.y) / (dSurface.z / zPos);
  vec3D_f pt = {pX, pY, zPos};
  pt = matInverse * pt;
  pt = pt + cameraPos;
  return pt;
}

void WavetableGraph::addPathFor(const vertex_arr_t& vertices) {
  auto* path = wavePaths.add(new wave_path_t());
  jassert(vertices.size() == WAVE_PATH_VERTS);
  path->zPosition = vertices[0].z;
  for (size_t i = 0; i < WAVE_PATH_VERTS; ++i) {
    path->path[i] = projectToCanvas(vertices[i]);
  }
  path->strokeWeight = flerp(2.0f, 4.5f, 1.0f - (path->zPosition - Z_SETBACK));
}

//===================================================

WavetableGraph::WavetableGraph(ElectrumState* s, int idx)
    : state(s), img(juce::Image::ARGB, GRAPH_W, GRAPH_H, true), oscID(idx) {
  // add itself as a listener to the graphingData
  state->graph.addListener(this);
  // initialize the rotation matrix
  const float xAngle = juce::MathConstants<float>::pi * 1.0f;
  const float yAngle = juce::MathConstants<float>::pi * -0.6f;
  const float zAngle = juce::MathConstants<float>::pi * -0.35f;
  matForward = Mat3x3::getRotationMatrix(xAngle, yAngle, zAngle);
  matInverse = Mat3x3::getRotationMatrix(-xAngle, -yAngle, -zAngle);
  // start the timer
  startTimerHz(GRAPH_REFRESH_HZ);
}

std::vector<vec3D_f> WavetableGraph::getVerticesForWave(
    const single_wave_norm_t& normPoints,
    float zPos) {
  std::vector<vec3D_f> out = {};
  out.push_back({0.0f, 1.0f, zPos});
  for (size_t i = 0; i < WAVE_GRAPH_POINTS; ++i) {
    const float x = (float)i / (float)WAVE_GRAPH_POINTS;
    out.push_back({x, normPoints[i], zPos});
  }
  out.push_back({1.0f, 1.0f, zPos});
  return out;
}

void WavetableGraph::loadVerticesForWave(vertex_arr_t& dest,
                                         const single_wave_norm_t& normPoints,
                                         float zPos) {
  dest[0] = {0.0f, 1.0f, zPos};
  for (size_t i = 0; i < WAVE_GRAPH_POINTS; ++i) {
    const float x = (float)i / (float)WAVE_GRAPH_POINTS;
    dest[i + 1] = {x, normPoints[i], zPos};
  }
  dest[WAVE_PATH_VERTS - 1] = {1.0f, 1.0f, zPos};
}

void WavetableGraph::graphingDataUpdated(GraphingData* gd) {
  const float newPos = gd->getOscPos(oscID);
  if (!fequal(newPos, currentPos)) {
    currentPos = newPos;
  }
}

void WavetableGraph::drawWaveGraph() {
  updateGraphImage();
  repaint();
  lastDrawnPos = currentPos;
  redrawRequested = false;
}

void WavetableGraph::timerCallback() {
  if (state->graph.wantsGraphPoints())
    return;
  if (wavePaths.size() < 1) {
    updateWavePaths(&state->graph);
    drawWaveGraph();
  } else if (!fequal(currentPos, lastDrawnPos)) {
    drawWaveGraph();
  }
}

void WavetableGraph::wavePointsUpdated(GraphingData* gd, int id) {
  DLog::log("Updating wave paths for oscillator: " + String(id));
  if (id == oscID) {
    updateWavePaths(gd);
    redrawRequested = true;
  }
}

void WavetableGraph::paint(juce::Graphics& g) {
  auto lBounds = getLocalBounds();
  g.drawImageWithin(img, lBounds.getX(), lBounds.getY(), lBounds.getWidth(),
                    lBounds.getHeight(), juce::RectanglePlacement::centred);
}

//==========================================================================

static vec3D_f lerp3D(const vec3D_f& a, const vec3D_f& b, float t) {
  return {flerp(a.x, b.x, t), flerp(a.y, b.y, t), flerp(a.z, b.z, t)};
}

void WavetableGraph::updateWavePaths(GraphingData* gd) {
  jassert(gd != nullptr);
  if (!wavePaths.isEmpty())
    wavePaths.clear();
  const int numWaves = gd->getNumWavesForOsc(oscID);
  DLog::log("Preparing to graph " + String(numWaves) + " waves");
  for (size_t i = 0; i < (size_t)numWaves; ++i) {
    gd->loadGraphPoints(waveNorm, oscID, (int)i);
    const float wavePos = (float)i / (float)numWaves;
    loadVerticesForWave(vertArray, waveNorm, wavePos + Z_SETBACK);
    addPathFor(vertArray);
  }
  if (!firstPathsGenerated) {
    DLog::log("Oscillator " + String(oscID) + " has " +
              String(wavePaths.size()) + " wave paths of an expected " +
              String(numWaves));
    firstPathsGenerated = true;
  }
}

wave_path_t WavetableGraph::getVirtualWave(

    float normPos) {
  if (wavePaths.size() < 2)
    return *wavePaths[0];
  // static bool firstVirtual = false;
  const float fIdx = normPos * (float)wavePaths.size();
  int lowIdx = std::min((int)std::floorf(fIdx), wavePaths.size() - 2);
  int highIdx = lowIdx + 1;
  float lerpAmt = fIdx - (float)lowIdx;
  WavePath2D path;
  // 1. calculate the points
  auto& lowPath = wavePaths[lowIdx]->path;
  auto& highPath = wavePaths[highIdx]->path;

  const float zPos = flerp(wavePaths[lowIdx]->zPosition,
                           wavePaths[lowIdx]->zPosition, lerpAmt);
  const float stroke = flerp(2.0f, 4.3f, zPos);
  for (size_t i = 0; i < WAVE_PATH_VERTS; ++i) {
    auto lowVert = vertexFromCanvas(lowPath[i], wavePaths[lowIdx]->zPosition);
    auto highVert = vertexFromCanvas(highPath[i], wavePaths[lowIdx]->zPosition);
    auto vVert = lerp3D(lowVert, highVert, lerpAmt);
    path[i] = projectToCanvas(vVert);
  }
  return {path, zPos, stroke};
}

void WavetableGraph::updateGraphImage() {
  img.clear(img.getBounds(), Color::black);
  juce::Graphics g(img);
  if (wavePaths.size() < 1)
    return;

  static color_t vWaveColor = Color::qualifierPurple;
  static color_t normWaveColor = Color::mintGreenPale;
  // 1. create the virtual wave object
  wave_path_t vWave;
  vWave.zPosition = Z_SETBACK + 0.04f;
  // auto vWave = getVirtualWave(currentPos);
  //   2. iterate through and draw each of the paths,
  //   starting from the highest Z position
  for (int i = wavePaths.size() - 1; i >= 0; --i) {
    // 1. draw this wave
    auto sColor = vWaveColor.interpolatedWith(
        normWaveColor, std::fabs(wavePaths[i]->zPosition - vWave.zPosition));
    g.setColour(sColor);
    wavePaths[i]->path.draw(g, wavePaths[i]->strokeWeight);
    // don't check the 0th wave for the vPath
    // if (i > 0) {
    //   if (wavePaths[i - 1]->zPosition <= vWave.zPosition &&
    //       wavePaths[i]->zPosition > vWave.zPosition) {
    //     g.setColour(vWaveColor);
    //     // vWave.path.draw(g, vWave.strokeWeight);
    //   }
    // }
  }
}

//===================================================

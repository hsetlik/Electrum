#include "Electrum/GUI/Wavetable/WavetableGraph.h"
#include <cmath>
#include "Electrum/GUI/LookAndFeel/Color.h"
#include "Electrum/Identifiers.h"
#include "Electrum/Shared/GraphingData.h"
// Stuff for 3D matrix math

template <typename T>
struct Mat3x3 {
  T data[3][3];
  Mat3x3() {
    for (int r = 0; r < 3; r++) {
      for (int c = 0; c < 3; c++) {
        data[r][c] = (T)0.0f;
      }
    }
  }

  Mat3x3(Mat3x3<T>& other) {
    for (int r = 0; r < 3; r++) {
      for (int c = 0; c < 3; c++) {
        data[r][c] = other.data[r][c];
      }
    }
  }

  juce::Vector3D<T> operator*(const juce::Vector3D<T>& vec) {
    juce::Vector3D<T> out = {0.0f, 0.0f, 0.0f};
    for (int c = 0; c < 3; c++) {
      out.x = out.x + (vec.x * data[0][c]);
      out.y = out.y + (vec.y * data[1][c]);
      out.z = out.z + (vec.z * data[2][c]);
    }
    return out;
  }

  juce::Vector3D<T> operator/(const juce::Vector3D<T>& vec) {
    juce::Vector3D<T> out = {0.0f, 0.0f, 0.0f};
    for (int c = 0; c < 3; c++) {
      out.x = out.x + (vec.x / data[0][c]);
      out.y = out.y + (vec.y / data[1][c]);
      out.z = out.z + (vec.z / data[2][c]);
    }
    return out;
  }

  Mat3x3<T> operator*(const Mat3x3<T>& other) {
    Mat3x3<T> out;
    auto& oData = other.data;
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        for (int u = 0; u < 3; u++) {
          out.data[i][j] += data[i][u] * oData[u][j];
        }
      }
    }
    return out;
  }

  void operator=(const Mat3x3<T>& other) {
    for (int r = 0; r < 3; r++) {
      for (int c = 0; c < 3; c++) {
        data[r][c] = other.data[r][c];
      }
    }
  }
  static Mat3x3<float> getRotationMatrix(float x, float y, float z) {
    Mat3x3<float> m1;

    m1.data[0][0] = 1.0f;
    m1.data[0][1] = 0.0f;
    m1.data[0][2] = 0.0f;

    m1.data[1][0] = 0.0f;
    m1.data[1][1] = std::cosf(x);
    m1.data[1][2] = std::sinf(x);

    m1.data[2][0] = 0.0f;
    m1.data[2][1] = std::sinf(x) * -1.0f;
    m1.data[2][2] = std::cosf(x);

    Mat3x3<float> m2;

    m2.data[0][0] = std::cosf(y);
    m2.data[0][1] = 0.0f;
    m2.data[0][2] = std::sinf(y) * -1.0f;

    m2.data[1][0] = 0.0f;
    m2.data[1][1] = 1.0f;
    m2.data[1][2] = 0.0f;

    m2.data[2][0] = 0.0f;
    m2.data[2][1] = std::sinf(x) * -1.0f;
    m2.data[2][2] = std::cosf(x);

    Mat3x3<float> m3;

    m3.data[0][0] = std::cosf(z);
    m3.data[0][1] = std::sinf(z);
    m3.data[0][2] = 0.0f;

    m3.data[1][0] = std::sinf(z) * -1.0f;
    m3.data[1][1] = std::cosf(z);
    m3.data[1][2] = 0.0f;

    m3.data[2][0] = 0.0f;
    m3.data[2][1] = 0.0f;
    m3.data[2][2] = 1.0f;

    return (m1 * m2) * m3;
  }
};

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
// the rotation matrix that projects our 3D points
// back onto a 2D bitmap
static Mat3x3<float> _graphRotationMatrix() {
  const float xAngle = juce::MathConstants<float>::pi * 1.0f;
  const float yAngle = juce::MathConstants<float>::pi * -0.6f;
  const float zAngle = juce::MathConstants<float>::pi * -0.35f;
  return Mat3x3<float>::getRotationMatrix(xAngle, yAngle, zAngle);
}
// various 3D math helpers
static std::vector<vec3D_f> getVerticesForWave(
    const single_wave_norm_t& normPoints,
    float zPos) {
  std::vector<vec3D_f> points = {};
  points.push_back({0.0f, 1.0f, zPos});
  for (size_t i = 0; i < WAVE_GRAPH_POINTS; ++i) {
    const float xPos = (float)i / (float)WAVE_GRAPH_POINTS;
    points.push_back({xPos, normPoints[i], zPos});
  }
  points.push_back({1.0f, 1.0f, zPos});
  return points;
}

static fpoint_t projectToCanvas(vec3D_f point3d) {
  static Mat3x3<float> rotation = _graphRotationMatrix();
  constexpr float yHeight = 1.4f;
  vec3D_f cameraPos = {-0.5f, yHeight, 0.0f};
  // represents the display surface
  vec3D_f dSurface = {0.0f, 0.0f, CAMERA_DISTANCE};

  // adjust by the camera position
  auto pt = point3d - cameraPos;
  // apply the rotation
  pt = rotation * pt;
  // back to 2d
  const float fX = ((dSurface.z / pt.z) * pt.x) + dSurface.x;
  const float fY = ((dSurface.z / pt.z) * pt.y) + dSurface.y;
  jassert(fX >= 0.0f && fX <= 1.0f);
  jassert(fY >= 0.0f && fY <= 1.0f);
  return {fX * (float)GRAPH_W, fY * (float)GRAPH_H};
}

// inverse of the above
static vec3D_f pointFromCanvas(juce::Point<float> f, float zPos) {
  static Mat3x3<float> rotation = _graphRotationMatrix();
  constexpr float yHeight = 1.4f;
  vec3D_f cameraPos = {-0.5f, yHeight, 0.0f};
  // represents the display surface
  vec3D_f dSurface = {0.0f, 0.0f, CAMERA_DISTANCE};

  // fX = ((dZ / pZ) * pX) + dX
  // pX = (fX - dX) / (dZ / pZ);
  const float pX = (f.x - dSurface.x) / (dSurface.z / zPos);
  const float pY = (f.y - dSurface.y) / (dSurface.z / zPos);
  vec3D_f pt = {pX, pY, zPos};
  pt = rotation / pt;
  return pt + cameraPos;
}

static wave_path_t convertToPath(const std::vector<vec3D_f>& vertices) {
  wave_path_t out;
  jassert(vertices.size() == WAVE_PATH_VERTS);
  for (size_t i = 0; i < vertices.size(); ++i) {
    out.path[i] = projectToCanvas(vertices[i]);
  }
  out.zPosition = vertices[0].z;
  out.strokeWeight = flerp(2.0f, 4.5f, 1.0f - out.zPosition);
  return out;
}

//===================================================

WavetableGraph::WavetableGraph(ElectrumState* s, int idx)
    : state(s), img(juce::Image::ARGB, GRAPH_W, GRAPH_H, true), oscID(idx) {
  // add itself as a listener to the graphingData
  state->graph.addListener(this);
  // start the timer
  startTimerHz(GRAPH_REFRESH_HZ);
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
  static bool firstPathsGenerated = false;
  if (!wavePaths.empty())
    wavePaths.clear();
  int numWaves = gd->getNumWavesForOsc(oscID);
  for (size_t i = 0; i < (size_t)numWaves; ++i) {
    const float wavePos = (float)i / (float)numWaves;
    auto wave = gd->getGraphPoints(oscID, (int)i);
    auto vertices = getVerticesForWave(wave, wavePos + Z_SETBACK);
    wavePaths.push_back(convertToPath(vertices));
  }
  if (!firstPathsGenerated) {
    DLog::log("Oscillator " + String(oscID) + " has " +
              String(wavePaths.size()) + " wave paths of an expected " +
              String(numWaves));
    firstPathsGenerated = true;
  }
}

static wave_path_t getVirtualWave(const std::vector<wave_path_t>& waves,
                                  float normPos) {
  if (waves.size() < 2)
    return waves[0];
  // static bool firstVirtual = false;
  const float fIdx = normPos * (float)waves.size();
  size_t lowIdx = std::min((size_t)std::floorf(fIdx), waves.size() - 2);
  size_t highIdx = lowIdx + 1;
  float lerpAmt = fIdx - (float)lowIdx;
  WavePath2D path;
  // 1. calculate the points
  auto& lowPath = waves[lowIdx].path;
  auto& highPath = waves[highIdx].path;

  const float zPos =
      flerp(waves[lowIdx].zPosition, waves[highIdx].zPosition, lerpAmt);
  const float stroke = flerp(2.0f, 4.3f, zPos);
  for (size_t i = 0; i < WAVE_PATH_VERTS; ++i) {
    auto lowVert = pointFromCanvas(lowPath[i], waves[lowIdx].zPosition);
    auto highVert = pointFromCanvas(highPath[i], waves[highIdx].zPosition);
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
  auto vWave = getVirtualWave(wavePaths, currentPos);
  // 2. iterate through and draw each of the paths,
  // starting from the highest Z position
  for (size_t i = wavePaths.size() - 1; i >= 0; --i) {
    // 1. draw this wave
    auto sColor = vWaveColor.interpolatedWith(
        normWaveColor, std::fabs(wavePaths[i].zPosition - vWave.zPosition));
    g.setColour(sColor);
    wavePaths[i].path.draw(g, wavePaths[i].strokeWeight);
    // don't check the 0th wave for the vPath
    if (i > 0) {
      if (wavePaths[i - 1].zPosition <= vWave.zPosition &&
          wavePaths[i].zPosition > vWave.zPosition) {
        g.setColour(vWaveColor);
        vWave.path.draw(g, vWave.strokeWeight);
      }
    }
  }
}

//===================================================

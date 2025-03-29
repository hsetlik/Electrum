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
  for (size_t i = 0; i < WAVE_GRAPH_POINTS; ++i) {
    const float xPos = (float)i / (float)WAVE_GRAPH_POINTS;
    points.push_back({xPos, normPoints[i], zPos});
  }
  return points;
}

static juce::Point<float> projectToCanvas(vec3D_f point3d) {
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
  jassert(fX >= 0.0f && fX < 1.0f);
  // jassert(fY >= 0.0f);
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
  juce::Path path;
  path.startNewSubPath(projectToCanvas(vertices[0]));
  for (size_t i = 1; i < vertices.size(); ++i) {
    path.lineTo(projectToCanvas(vertices[i]));
  }
  path.closeSubPath();
  const float zPos = vertices[0].z;
  const float strokeWeight = flerp(2.0f, 4.5f, 1.0f - zPos);
  return {path, zPos, strokeWeight};
}

//===================================================

WavetableGraph::WavetableGraph(ElectrumState* s, int idx)
    : state(s), img(juce::Image::ARGB, GRAPH_W, GRAPH_H, true), oscID(idx) {
  // add itself as a listener to the graphingData
  state->graph.addListener(this);
  // if its wave points already exist we can init this
  if (!state->graph.wantsGraphPoints()) {
    updateWavePaths(&state->graph);
  }
  startTimerHz(GRAPH_REFRESH_HZ);
  redrawRequested = true;
}

void WavetableGraph::graphingDataUpdated(GraphingData* gd) {
  const float newPos = gd->getOscPos(oscID);
  if (!fequal(newPos, currentPos)) {
    currentPos = newPos;
  }
}

void WavetableGraph::timerCallback() {
  if (!fequal(lastDrawnPos, currentPos) || redrawRequested) {
    updateGraphImage();
    repaint();
    lastDrawnPos = currentPos;
    redrawRequested = false;
  }
}

void WavetableGraph::wavePointsUpdated(GraphingData* gd, int id) {
  // DLog::log("Updating wave paths for oscillator: " + String(id));
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

void WavetableGraph::updateWavePaths(GraphingData* gd) {
  static bool firstPathsGenerated = false;
  wavePaths.clear();
  for (size_t i = 0; i < gd->graphPoints[oscID].waves.size(); ++i) {
    const float wavePos = (float)i / (float)gd->graphPoints[oscID].waves.size();
    auto& wave = gd->graphPoints[oscID].waves[i];
    auto vertices = getVerticesForWave(wave, wavePos + Z_SETBACK);
    wavePaths.push_back(convertToPath(vertices));
  }
  if (!firstPathsGenerated) {
    DLog::log("Generated " + String(wavePaths.size()) + " wave paths");
    firstPathsGenerated = true;
  }
}

static wave_path_t getVirtualWave(const std::vector<wave_path_t>& waves,
                                  float normPos) {
  if (waves.size() < 2)
    return waves[0];

  static bool firstVirtual = false;
  const float fIdx = normPos * (float)waves.size();
  size_t lowIdx = std::min((size_t)std::floorf(fIdx), waves.size() - 2);
  size_t highIdx = lowIdx + 1;
  float lerpAmt = fIdx - (float)lowIdx;
  juce::Path vPath;
  const float zPos = normPos + Z_SETBACK;
  // 1. calculate the path points
  auto& lowPath = waves[lowIdx].path;
  auto& highPath = waves[highIdx].path;
  auto lo3D =
      pointFromCanvas(lowPath.getPointAlongPath(0.0f), waves[lowIdx].zPosition);
  auto hi3D = pointFromCanvas(highPath.getPointAlongPath(0.0f),
                              waves[highIdx].zPosition);
  vec3D_f startVert = {0.0f, flerp(lo3D.y, hi3D.y, lerpAmt), zPos};
  vPath.startNewSubPath(projectToCanvas(startVert));
  for (int i = 1; i < WAVE_GRAPH_POINTS; ++i) {
    float pRelative = (float)i / (float)WAVE_GRAPH_POINTS;
    lo3D = pointFromCanvas(lowPath.getPointAlongPath(pRelative),
                           waves[lowIdx].zPosition);
    hi3D = pointFromCanvas(highPath.getPointAlongPath(pRelative),
                           waves[highIdx].zPosition);
    vec3D_f vert = {pRelative, flerp(lo3D.y, hi3D.y, lerpAmt), zPos};
    vPath.lineTo(projectToCanvas(vert));
  }
  // figure out the line width
  static frange_t strokeRange(1.8f, 4.0f);
  const float weight = strokeRange.convertFrom0to1(1.0f - normPos);
  if (!firstVirtual) {
    DLog::log("Generated first virtual wave");
    firstVirtual = true;
  }
  return {vPath, zPos, weight};
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
    juce::PathStrokeType pst(wavePaths[i].strokeWeight);
    g.setColour(sColor);
    g.strokePath(wavePaths[i].path, pst);
    // don't check the 0th wave for the vPath
    if (i > 0) {
      if (wavePaths[i - 1].zPosition <= vWave.zPosition &&
          wavePaths[i].zPosition > vWave.zPosition) {
        g.setColour(vWaveColor);
        juce::PathStrokeType vPst(vWave.strokeWeight);
        g.strokePath(vWave.path, vPst);
      }
    }
  }
}

//===================================================

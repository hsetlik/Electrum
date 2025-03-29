#include "Electrum/GUI/Wavetable/WavetableGraph.h"
#include "Electrum/GUI/LookAndFeel/Color.h"
#include "Electrum/Identifiers.h"
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
    const std::vector<float>& normPoints,
    float zPos) {
  std::vector<vec3D_f> points = {};
  for (size_t i = 0; i < normPoints.size(); ++i) {
    const float xPos = (float)i / (float)normPoints.size();
    points.push_back({xPos, normPoints[i], zPos});
  }
  return points;
}

static juce::Point<float> projectToCanvas(vec3D_f point3d) {
  static Mat3x3<float> rotation = _graphRotationMatrix();
  constexpr float yHeight = 1.4f;
  vec3D_f cameraPos = {-0.5f, yHeight, 0.0f};
  // represents the display surface
  vec3D_f e = {0.0f, 0.0f, CAMERA_DISTANCE};

  // adjust by the camera position
  auto pt = point3d - cameraPos;
  // apply the rotation
  pt = rotation * pt;
  // back to 2d
  const float fX = ((e.z / pt.z) * pt.x) + e.x;
  const float fY = ((e.z / pt.z) * pt.y) + e.y;
  return {fX * (float)GRAPH_W, fY * (float)GRAPH_H};
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
  return {path, Color::mintGreenBright, zPos, strokeWeight};
}

//===================================================

WavetableGraph::WavetableGraph(ElectrumState* s, int idx)
    : state(s), img(juce::Image::ARGB, GRAPH_W, GRAPH_H, true), oscID(idx) {
  // add itself as a listener to the graphingData
  state->graph.addListener(this);
  startTimerHz(GRAPH_REFRESH_HZ);
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

void WavetableGraph::loadWaveDataFrom(Wavetable* wt) {
  // so this needs (for each wave)
  // 1. convert to a list of GRAPH_W floats in the range 0-1
  // 2. assign the wave a z-plane position based on its position in the table
  // 3. convert the norm. wave values and the z-plane into a list of 3D points
  // 4. convert that to a 'wave_path_t' object and add it to the stack
  //--------
  // clear out wavePaths if it's not empty already
  while (!wavePaths.empty()) {
    wavePaths.pop();
  }
  for (int wave = 0; wave < wt->size(); ++wave) {
    auto normValues = wt->normVectorForWave(wave, GRAPH_W);
    const float zPos = ((float)wave / (float)wt->size()) + Z_SETBACK;
    auto vertices = getVerticesForWave(normValues, zPos);
  }
}

#pragma once

#include <stack>
#include "Electrum/Audio/Wavetable.h"
#include "Electrum/GUI/LookAndFeel/Color.h"
#include "Electrum/Shared/GraphingData.h"
#include "Electrum/Shared/ElectrumState.h"

#include "juce_opengl/juce_opengl.h"

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
  static Mat3x3<float> getRotationMatrix(float x, float y, float z);
};

//==================================================================
typedef juce::Vector3D<float> vec3D_f;

#define GRAPH_REFRESH_HZ 24
#define GRAPH_W 512
#define GRAPH_H 512

#define Z_SETBACK 0.6f
#define CAMERA_DISTANCE 0.3f

struct wave_path_t {
  juce::Path path;
  color_t color;
  float zPosition;
  float strokeWeight;
};

class WavetableGraph : public GraphingData::Listener,
                       public juce::Component,
                       public juce::Timer {
private:
  ElectrumState* const state;
  juce::Image img;
  bool wantsWaveData = true;
  bool redrawRequested = false;
  // position state stuff
  float lastDrawnPos = 0.0f;
  float currentPos = 0.0f;
  // 3D projection stuff
  Mat3x3<float> rotationMatrix;
  // and the sets of paths we need to draw
  std::stack<wave_path_t> wavePaths;

public:
  const int oscID;
  WavetableGraph(ElectrumState* s, int idx);
  ~WavetableGraph() override;
  bool needsWaveData() const { return wantsWaveData; }
  void graphingDataUpdated(GraphingData* gd) override;
  void timerCallback() override;
  // this needs to be called at startup and
  // then whenever a new wave is selected
  void loadWaveDataFrom(Wavetable* wt);

private:
  void setRotationMatrix();
  void updateGraphImage();
};

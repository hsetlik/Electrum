#pragma once

#include "juce_opengl/juce_opengl.h"

struct Mat3x3 {
  float data[3][3];
  Mat3x3() {
    for (int r = 0; r < 3; r++) {
      for (int c = 0; c < 3; c++) {
        data[r][c] = 0.0f;
      }
    }
  }

  Mat3x3(Mat3x3& other) {
    for (int r = 0; r < 3; r++) {
      for (int c = 0; c < 3; c++) {
        data[r][c] = other.data[r][c];
      }
    }
  }

  juce::Vector3D<float> operator*(const juce::Vector3D<float>& vec) {
    juce::Vector3D<float> out = {0.0f, 0.0f, 0.0f};
    for (int c = 0; c < 3; c++) {
      out.x = out.x + (vec.x * data[0][c]);
      out.y = out.y + (vec.y * data[1][c]);
      out.z = out.z + (vec.z * data[2][c]);
    }
    return out;
  }

  juce::Vector3D<float> operator/(const juce::Vector3D<float>& vec) {
    juce::Vector3D<float> out = vec;
    for (int c = 2; c >= 0; --c) {
      out.x = out.x - (vec.x * data[0][c]);
      out.y = out.y - (vec.x * data[1][c]);
      out.z = out.z - (vec.z * data[2][c]);
    }
    return out;
  }

  Mat3x3 operator*(const Mat3x3& other) {
    Mat3x3 out;
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

  void operator=(const Mat3x3& other) {
    for (int r = 0; r < 3; r++) {
      for (int c = 0; c < 3; c++) {
        data[r][c] = other.data[r][c];
      }
    }
  }
  //==================================================================
  static Mat3x3 getRotationMatrix(float x, float y, float z);
};

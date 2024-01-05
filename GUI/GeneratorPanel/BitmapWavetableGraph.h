#pragma once
#include "../../Parameters/ElectrumValueTree.h"
#include "../../Parameters/Identifiers.h"
#include "../Color.h"
// some stuff to help us with the 3D projection math
template <typename T> struct Mat3x3
{
  T data[3][3];
  Mat3x3()
  {
    for (int r = 0; r < 3; r++)
    {
      for (int c = 0; c < 3; c++)
      {
        data[r][c] = (T)0.0f;
      }
    }
  }

  Mat3x3(Mat3x3<T> &other)
  {
    for (int r = 0; r < 3; r++)
    {
      for (int c = 0; c < 3; c++)
      {
        data[r][c] = other.data[r][c];
      }
    }
  }

  Vector3D<T> operator*(const Vector3D<T> &vec)
  {
    Vector3D<T> out = {0.0f, 0.0f, 0.0f};
    for (int c = 0; c < 3; c++)
    {
      out.x = out.x + (vec.x * data[0][c]);
      out.y = out.y + (vec.y * data[1][c]);
      out.z = out.z + (vec.z * data[2][c]);
    }
    return out;
  }

  Mat3x3<T> operator*(const Mat3x3<T> &other)
  {
    Mat3x3<T> out;
    auto &oData = other.data;
    for (int i = 0; i < 3; i++)
    {
      for (int j = 0; j < 3; j++)
      {
        for (int u = 0; u < 3; u++)
        {
          out.data[i][j] += data[i][u] * oData[u][j];
        }
      }
    }
    return out;
  }

  void operator=(const Mat3x3<T> &other)
  {
    for (int r = 0; r < 3; r++)
    {
      for (int c = 0; c < 3; c++)
      {
        data[r][c] = other.data[r][c];
      }
    }
  }
  //==================================================================
  static Mat3x3<float> getRotationMatrix(float x, float y, float z);
};

#define GRAPH_REFRESH_HZ 24
#define GRAPH_W 512
#define GRAPH_H 512

#define Z_SETBACK 0.75f
#define CAMERA_DISTANCE 0.35f

class BitmapWavetableGraph : public Component, public Timer
{
private:
  EVT *const state;
  const int index;
  // this is maybe weird but I'm trying it
  float currentZCoeff;
  float maxZCoeff;
  float minZCoeff;
  bool zCoeffIncreasing;

  Image img;
  // keep track of whether we need to update the image
  float lastWavePos;
  bool needsImgUpdate;
  // just so we don't have to recalculate the rotation matrix for every point
  Mat3x3<float> rotation;
  // this gets called in the constructor
  void setRotationMatrix();
  // this stores our paths. makes sense in updateImagePixels
  // a little struct for storing everything we need to draw a given wave
  struct WavePathData
  {
    Path path;
    Colour color;
    float zPos;
  };
  std::stack<WavePathData> wavePaths;
  // this generates a list of vertices based on the provided Wave and z values
  static std::vector<Vector3D<float>> createVerticesFor(Wave &wave, int numPoints, float zPlane);
  // converts the list of vertices from the above function into a Path object we can draw
  Path convertToPath(std::vector<Vector3D<float>> &v);
  // this does the actual projection matrix math for the above function
  Point<float> projectToCanvas(Vector3D<float> point);

  // rendering logic goes here
  void updateImagePixels();

public:
  BitmapWavetableGraph(EVT *tree, int idx);
  void timerCallback() override;
  void paint(Graphics &g) override;
  // use this to manually trigger a redraw of the image. e.g. when a new wavetable is chosen
  void triggerRefresh() { needsImgUpdate = true; }
};

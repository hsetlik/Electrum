#include "BitmapWavetableGraph.h"

template <typename T> Mat3x3<float> Mat3x3<T>::getRotationMatrix(float x, float y, float z)
{
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

  return m1 * m2 * m3;
}
//==================================================================================
BitmapWavetableGraph::BitmapWavetableGraph(EVT *tree, int idx)
    : state(tree), index(idx), img(Image::ARGB, GRAPH_W, GRAPH_H, true), lastWavePos(0.0f),
      needsImgUpdate(false)
{
  // calculate our rotation matrix now bc it's math-intensive and won't change
  const float xAngle = MathConstants<float>::pi * -0.03f;
  const float yAngle = MathConstants<float>::pi * (-0.08f * (float)(index + 1));
  const float zAngle = MathConstants<float>::pi * 0.0f;
  rotation = Mat3x3<float>::getRotationMatrix(xAngle, yAngle, zAngle);
  // start the timer
  startTimerHz(GRAPH_REFRESH_HZ);
}

void BitmapWavetableGraph::timerCallback()
{
  float currentPos = state->getLeadingVoiceOscPosition(index);
  if (currentPos != lastWavePos || needsImgUpdate)
  {
    lastWavePos = currentPos;
    updateImagePixels();
    repaint();
    needsImgUpdate = false;
  }
}

void BitmapWavetableGraph::paint(Graphics &g)
{
  auto fBounds = getLocalBounds().toFloat();
  g.drawImage(img, fBounds);
}
//==================================================================================
void BitmapWavetableGraph::updateImagePixels()
{
  // clear the image
  img.clear(img.getBounds(), Color::black);
  Graphics g(img);
  // grip the wave shapes for the current oscillator
  auto waves = state->getAudioData()->getBaseWaves(index);
  for (size_t i = 0; i < waves.size(); i++)
  {
    // calculate the vertices for this wave
    float zPos = (float)i / (float)waves.size();
    auto w = createVerticesFor(waves[i], 128, zPos + Z_SETBACK);
    // convert to a path and figure out the stroke
    auto wavePath = convertToPath(w);
    float strokeW = (1.8f * (1.0f - zPos));
    PathStrokeType pst(strokeW);

    g.setColour(Color::paleOrange.brighter());
    g.strokePath(wavePath, pst);
  }
}

Path BitmapWavetableGraph::convertToPath(std::vector<Vector3D<float>> &vertices)
{
  Path p;
  p.startNewSubPath(projectToCanvas(vertices[0]));
  for (size_t i = 1; i < vertices.size(); i++)
  {
    p.lineTo(projectToCanvas(vertices[i]));
  }
  p.closeSubPath();
  return p;
}

std::vector<Vector3D<float>> BitmapWavetableGraph::createVerticesFor(Wave &wave, int numPoints,
                                                                     float zPlane)
{
  std::vector<Vector3D<float>> verts;
  verts.push_back({0.0f, 0.0f, zPlane});
  for (int i = 0; i < numPoints; ++i)
  {
    float x = (float)i / (float)numPoints;
    float wVal = WaveUtil::valueAtPhase(wave, x);
    float y = (wVal + 1.0f) / 2.0f;
    verts.push_back({x, y, zPlane});
  }
  verts.push_back({1.0f, 0.0f, zPlane});
  return verts;
}

Point<float> BitmapWavetableGraph::projectToCanvas(Vector3D<float> point)
{
  Vector3D<float> c = {-0.5f, 0.3f, 0.0f}; // represents the camera pinhole
  Vector3D<float> e = {0.0f, 0.0f, 0.45f}; // represents the display surface relative to the camera
  // correct for the camera pos
  auto d = point - c;
  // multiply by the rotation matrix
  d = rotation * d;
  // now convert to the 2d plane
  float xPos = ((e.z / d.z) * d.x) + e.x;
  float yPos = ((e.z / d.z) * d.y) + e.y;

  return {(1.0f - xPos) * (float)GRAPH_W, (1.0f - yPos) * (float)GRAPH_H};
}

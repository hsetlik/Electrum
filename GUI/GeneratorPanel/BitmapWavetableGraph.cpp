#include "BitmapWavetableGraph.h"
//==================================================================================
BitmapWavetableGraph::BitmapWavetableGraph(EVT *tree, int idx)
    : state(tree), index(idx), img(Image::ARGB, GRAPH_W, GRAPH_H, true), lastWavePos(0.0f),
      needsImgUpdate(false)
{

  setRotationMatrix();
  // start the timer
  startTimerHz(GRAPH_REFRESH_HZ);
}

void BitmapWavetableGraph::setRotationMatrix()
{
  const float xAngle = MathConstants<float>::pi * 1.0f;
  const float yAngle = MathConstants<float>::pi * -0.6f;
  const float zAngle = MathConstants<float>::pi * -0.35f;
  rotation = Mat3x3<float>::getRotationMatrix(xAngle, yAngle, zAngle);
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
  // STEP 1: calculate the vertices and paths and add them to the stack
  const int numVertices = 65;
  // grip the wave shapes for the current oscillator
  auto waves = state->getAudioData()->getBaseWaves(index);
  for (size_t i = 0; i < waves.size(); i++)
  {
    float currentZ = (float)i / (float)waves.size();
    // after the first wave we need to start checking if it's time to draw the virtual wave yet
    if (i > 0)
    {
      float prevZ = (float)(i - 1) / (float)waves.size();
      if (prevZ < lastWavePos && currentZ >= lastWavePos)
      {
        auto wave = WaveUtil::interpolateWaveSet(waves, lastWavePos);
        auto verts = createVerticesFor(wave, numVertices, lastWavePos + Z_SETBACK);
        auto p = convertToPath(verts);
        auto col = Color::brightSeafoam.withAlpha(0.9f);
        float z = lastWavePos + Z_SETBACK;
        wavePaths.push({p, col, z, 4.5f - z});
      }
    }
    // create our vertices and path
    auto vertices = createVerticesFor(waves[i], numVertices, currentZ + Z_SETBACK);
    auto path = convertToPath(vertices);

    // calculate the color
    float proximity = std::fabs(lastWavePos - currentZ) * 2.0f;
    const Colour bright =
        Color::brightSeafoam.interpolatedWith(Color::paleSeafoam, 0.5f).withAlpha(0.8f);
    const Colour dark = bright.darker(0.3f).withAlpha(0.65f);
    Colour waveColor = bright.interpolatedWith(dark, proximity);
    // create the WavePathData object and add it to our stack
    float zFinal = currentZ + Z_SETBACK;
    WavePathData d{path, waveColor, zFinal, 3.0f - zFinal};
    wavePaths.push(d);
  }
  // STEP 2: go through the stack and draw each wave
  img.clear(img.getBounds(), Color::black);
  Graphics g(img);
  while (!wavePaths.empty())
  {
    auto w = wavePaths.top();
    PathStrokeType pst(w.stroke);
    g.setColour(w.color);
    g.strokePath(w.path, pst);
    wavePaths.pop();
  }

  // make sure we emptied the stack
  if (!wavePaths.empty())
  {
    DLog::log("Warning! " + String(wavePaths.size()) + " paths not rendered!");
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
    jassert(x >= 0.0f && x <= 1.0f);
    float wVal = WaveUtil::valueAtPhase(wave, x);
    float y = (wVal + 1.0f) / 2.0f;
    verts.push_back({x, y, zPlane});
  }
  verts.push_back({1.0f, 0.0f, zPlane});
  return verts;
}

Point<float> BitmapWavetableGraph::projectToCanvas(Vector3D<float> point)
{
  float yHeight = 1.4f;
  Vector3D<float> c = {-0.5f, yHeight, 0.0f}; // represents the camera pinhole
  Vector3D<float> e = {0.0f, 0.0f,
                       CAMERA_DISTANCE}; // represents the display surface relative to the camera
  // correct for the camera pos
  auto d = point - c;
  // multiply by the rotation matrix
  d = rotation * d;
  // now convert to the 2d plane
  float xPos = ((e.z / d.z) * d.x) + e.x;
  float yPos = ((e.z / d.z) * d.y) + e.y;

  return {(xPos) * (float)GRAPH_W, yPos * (float)GRAPH_H};
}

//=======================================================================================================
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

  return (m1 * m2) * m3;
}

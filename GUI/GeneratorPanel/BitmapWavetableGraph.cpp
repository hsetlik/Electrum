#include "BitmapWavetableGraph.h"

BitmapWavetableGraph::BitmapWavetableGraph(EVT *tree, int idx)
    : state(tree), index(idx), img(Image::ARGB, GRAPH_W, GRAPH_H, true), lastWavePos(0.0f),
      needsImgUpdate(false)
{
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
  // grip the wave shapes for the current oscillator
  auto waves = state->getAudioData()->getBaseWaves(index);
  for (size_t i = 0; i < waves.size(); i++)
  {
    // calculate the vertices for this wave
    float zPos = (float)i / (float)waves.size();
    auto waveVertices = createVerticesFor(waves[i], 128, zPos);
    // divide the list of vertices into line segments and draw each
    for (size_t v = 1; v < waveVertices.size(); v++)
    {
      auto first = waveVertices[v - 1];
      auto second = waveVertices[v];
    }
  }
}
std::vector<Vertex> BitmapWavetableGraph::createVerticesFor(Wave &wave, int numPoints, float zPlane)
{
  std::vector<Vertex> verts;
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

Pixel BitmapWavetableGraph::projectVertex(const Vertex &v, int imgWidth, int imgHeight) {}

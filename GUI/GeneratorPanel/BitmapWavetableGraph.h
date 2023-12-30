#pragma once
#include "../../Parameters/ElectrumValueTree.h"
#include "../../Parameters/Identifiers.h"
#include "../Color.h"

#define GRAPH_REFRESH_HZ 24
#define GRAPH_W 512
#define GRAPH_H 512

struct Vertex
{
  float x;
  float y;
  float z;
};

struct Pixel
{
  int x;
  int y;
};

class BitmapWavetableGraph : public Component, public Timer
{
private:
  EVT *const state;
  const int index;

  Image img;
  // keep track of whether we need to update the image
  float lastWavePos;
  bool needsImgUpdate;
  // this generates a list of vertices based on the provided Wave and z values
  static std::vector<Vertex> createVerticesFor(Wave &wave, int numPoints, float zPlane);
  //  this function handles the projection math
  static Pixel projectVertex(const Vertex &v, int imgWidth, int imgHeight);
  // this is my dubious-at-best way of drawing projected 3D lines.
  static void draw3DLine(Image &image, Vertex start, Vertex end, const Colour &color,
                         float strokeWidth);

  // rendering logic goes here
  void updateImagePixels();

public:
  BitmapWavetableGraph(EVT *tree, int idx);
  void timerCallback() override;
  void paint(Graphics &g) override;
  // use this to manually trigger a redraw of the image. e.g. when a new wavetable is chosen
  void triggerRefresh() { needsImgUpdate = true; }
};

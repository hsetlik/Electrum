#include "Electrum/GUI/Wavetable/WavetableGraph.h"

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

static wave_path_t convertToPath(const std::vector<vec3D_f>& vertices) {}

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

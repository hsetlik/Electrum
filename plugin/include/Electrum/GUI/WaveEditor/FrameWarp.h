#pragma once
#include "Electrum/Audio/Wavetable.h"
#include "Electrum/GUI/GUITypedefs.h"
#include "Electrum/GUI/WaveEditor/EditValueTree.h"
#include "Electrum/Identifiers.h"

// represents a single user-controlled point on the
struct warp_point_t {
  float frequency;
  float magnitude;
  bool operator<(const warp_point_t& other) {
    return frequency < other.frequency;
  }
  static warp_point_t fromValueTree(const ValueTree& vt);
  static ValueTree toValueTree(const warp_point_t& point);
};

// this class holds all the state we need for
// the parametric point graph. the component should have a unique_ptr
// to one of these that will get replaced every time a new frame is
// selected
class FrameWarp : juce::AsyncUpdater {
private:
  struct neighbor_pair_t {
    warp_point_t* left;
    warp_point_t* right;
  };
  bool binsReady = false;
  neighbor_pair_t getNeighborsForFreq(float freq);
  ValueTree loadedWaveTree;
  bin_array_t savedBins;
  bin_array_t workingBins;
  std::vector<warp_point_t> points;
  float maxMagnitude = 1000.0f;
  frange_t magnitudeRange;
  float inMagRange(float mag) const;

  void sortPoints();

public:
  FrameWarp(ValueTree& vt);
  ValueTree getWarpTree() {
    return loadedWaveTree.getChildWithName(WaveEdit::FFT_WARP);
  }
  void handleAsyncUpdate() override;
  // the main functions for adding, editing, and removing warp points
  void createWarpPoint(float normFreq);
  // check if a point can be moved thusly
  bool canPointHaveFrequency(warp_point_t* freq, float normFreq);
  void placePoint(warp_point_t* point, float normMagnitude, float freq);
  void deletePoint(warp_point_t* point);
  // returns either the editable point within some distance of this point, or
  // nullptr is there is none
  warp_point_t* editablePointNear(float normMagnitude, float freq);
  // mouse callbacks should use this when checking to create a
  // point
  bool isNearEditLine(float normMagnitude, float freq) const;
  // the main graph drawing happens here
  void drawSpectrumRange(juce::Graphics& g,
                         const frect_t& fBounds,
                         float nFreqStart,
                         float nFreqEnd,
                         warp_point_t* selectedPt = nullptr);

private:
  juce::Path makeWindowedPath(const bin_array_t& bins,
                              float freqStart,
                              float freqEnd,
                              const frect_t& imgBounds);
  struct warp_handle_t {
    fpoint_t point;
    bool selected;
  };
  void drawEditPoints(juce::Graphics& g,
                      const frect_t& bounds,
                      float freqStart,
                      float freqEnd,
                      warp_point_t* selectedPt);
};

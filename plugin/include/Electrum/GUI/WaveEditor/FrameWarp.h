#pragma once
#include "Electrum/Audio/Wavetable.h"
#include "Electrum/GUI/GUITypedefs.h"
#include "Electrum/GUI/LookAndFeel/Color.h"
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

struct bin_area_t {
  frect_t bounds;
  size_t idx;
};

// this class holds all the state we need for
// the parametric point graph. the component should have a unique_ptr
// to one of these that will get replaced every time a new frame is
// selected
class FrameWarp : juce::AsyncUpdater {
private:
  juce::CriticalSection criticalSection;
  struct bin_pair_t {
    size_t low;
    size_t high;
  };
  bool binsReady = false;
  ValueTree loadedWaveTree;
  bin_array_t savedBins;
  bin_array_t workingBins;
  std::vector<warp_point_t> points;
  float maxMagnitude = 1000.0f;
  frange_t magnitudeRange;
  float magnitudeToNorm(float mag) const;

  void sortPoints();

  fpoint_t warpPointToBounds(const frect_t& bounds,
                             const warp_point_t& wp) const;
  size_t indexOf(warp_point_t* pt) const;

  // this can replace the ugly external call to 'canPointHaveFrequency'
  bool isMovementLegal(warp_point_t* pt, float normMagnitude, float freq) const;
  warp_point_t* closestEditPoint(size_t bin);
  float getInfluenceAmt(warp_point_t* point, size_t binIdx);
  size_t minDistanceToNeutral(size_t pointIdx);
  // helper for actual warping
  float warpBinMagnitude(size_t binIdx);

public:
  FrameWarp(ValueTree& vt);
  // return the child tree of type FFT_WARP to be appended/repaced on
  // a WAVE_FRAME parent
  ValueTree getWarpTree(bool includeWaveString) const;

  void handleAsyncUpdate() override;
  // the main functions for adding, editing, and removing warp points
  void createWarpPoint(float normMagnitude, float freq);
  // check if a point can be moved thusly
  void placePoint(warp_point_t* point, float normMagnitude, float freq);
  void deletePoint(warp_point_t* point);
  // returns either the editable point within some distance of this point, or
  // nullptr is there is none
  warp_point_t* editablePointNear(const frect_t& bounds,
                                  const fpoint_t& point,
                                  float thresh = 5.0f);
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

  std::vector<bin_area_t> getVisibleBinAreas(const bin_array_t& bins,
                                             float fStart,
                                             float fEnd,
                                             const frect_t& bounds);
  void drawBinsFixedColor(juce::Graphics& g,
                          const bin_array_t& bins,
                          const frect_t& bounds,
                          float fStart,
                          float fEnd,
                          const color_t& color);
  void drawBinsPhaseColors(juce::Graphics& g,
                           const bin_array_t& bins,
                           const frect_t& bounds,
                           float fStart,
                           float fEnd);

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

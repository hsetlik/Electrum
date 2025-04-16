#pragma once
#include "Electrum/Audio/Wavetable.h"
#include "Electrum/GUI/GUITypedefs.h"
#include "Electrum/GUI/WaveEditor/WaveEdiorContext.h"

#define MIN_WARP_PTS 5
#define MAX_WARP_PTS 200
#define DEFAULT_WARP_POINTS 30

class WarpPoint {
public:
  float normFreq;
  const float originalMag;
  float warpedMag;
  WarpPoint(float nf, float mag) : normFreq(nf), originalMag(mag) {
    warpedMag = originalMag;
  }
};

#define MIN_FFT_ZOOM 0.3f
#define MAX_FFT_ZOOM 7.0f
#define MAX_CLICK_DISTANCE 6.0f
class FrameWarp;
#define FFT_GRAPH_RES 512
#define WAVE_UPDATE_HZ 2
class FrameSpectrum : public WaveEditListener, public juce::Timer {
  static constexpr int minWidth = FFT_GRAPH_RES;
  bin_array_t loadedWaveBins;
  bin_array_t warpWaveBins;
  bool binsReady = false;
  bool warpUpdateNeeded = false;
  frange_t currentMagRange;
  int warpPoints = -1;
  int currentFrame = -1;
  float currentMaxMagnitude = 0.0f;
  float currentZoom = 1.0f;

public:
  FrameSpectrum(ValueTree& vt);
  void setZoomNorm(float v);
  int numWarpPoints() const { return warpPoints; }
  void setNumWarpPoints(int points);
  void frameWasFocused(int idx) override;
  void paint(juce::Graphics& g) override;
  void timerCallback() override;
  void resized() override;
  // mouse callbacks
  void mouseDown(const juce::MouseEvent& m) override;
  void mouseUp(const juce::MouseEvent& m) override;
  void mouseDoubleClick(const juce::MouseEvent& m) override;
  void mouseDrag(const juce::MouseEvent& m) override;

  friend class FrameWarp;

private:
  std::unique_ptr<FrameWarp> warp;
  WarpPoint* selectedPoint = nullptr;
  // internal warp point logic
  float yPosLoaded(const frect_t& bounds, float fNorm) const;
  float yPosWarped(const frect_t& bounds, float fNorm) const;
  // the main point/parameter mapping functions
  float yPosForMag(const frect_t& bounds, float magnitude) const;
  float magForYPos(const frect_t& bounds, float yPos) const;
  fpoint_t warpPointToCanvas(const frect_t& bounds, WarpPoint* p);
  // checks if the point is on the edge of the graph
  bool isNearEditLine(const frect_t& bounds, fpoint_t point);
};
//===============================================
// this guy keeps track of all our edits to the the spectrum,
// handles the business of calculating the Magnitude for each bin
class FrameWarp {
private:
  FrameSpectrum* const parent;
  juce::OwnedArray<WarpPoint> touchedPoints;
  std::vector<WarpPoint*> editPoints;
  void sortEditPoints();

public:
  int numEditPoints() const { return (int)editPoints.size(); }
  WarpPoint* getEditPoint(int idx) { return editPoints[(size_t)idx]; }
  FrameWarp(FrameSpectrum* p);
  // the parent should call this to update its 'warpWaveBins'.
  // this is where the main spectral warping happens
  void updateWarpBins(const bin_array_t& loaded, bin_array_t& warped);
  // returns the legal WarpPoint at the given point or nullptr if there is none
  WarpPoint* getPointNear(const frect_t& bounds, const fpoint_t& point);
  // lets the parent know whether a WarpPoint can legally be moved to this
  // frequency
  bool canMoveTo(WarpPoint* pt, float freq);
  // create a new warp point at the given frequency/magnitude
  void createWarpPoint(float nFreq, float mag);
  void removeWarpPoint(WarpPoint* pt);
};

//=============================================
class FrameSpectrumViewer : public Component {
private:
  FrameSpectrum spec;
  juce::Slider zoomSlider;
  BoundedAttString zoomStr;
  juce::Slider pointSlider;
  BoundedAttString pointStr;
  juce::Viewport vpt;

public:
  FrameSpectrumViewer(ValueTree& vt);
  void resized() override;
  void paint(juce::Graphics& g) override;
};

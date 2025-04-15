#pragma once
#include "Electrum/Audio/Wavetable.h"
#include "Electrum/GUI/GUITypedefs.h"
#include "Electrum/GUI/WaveEditor/WaveEdiorContext.h"

#define MIN_WARP_PTS 5
#define MAX_WARP_PTS 200
#define DEFAULT_WARP_POINTS 30

class WarpPoint {
public:
  const float normFreq;
  const float originalMag;
  float warpedMag;
  WarpPoint(float nf, float mag) : normFreq(nf), originalMag(mag) {
    warpedMag = originalMag;
  }
};

#define MIN_FFT_ZOOM 0.3f
#define MAX_FFT_ZOOM 7.0f
class FrameWarp;
#define FFT_GRAPH_RES AUDIBLE_BINS
class FrameSpectrum : public WaveEditListener {
  static constexpr int minWidth = FFT_GRAPH_RES;
  bin_array_t loadedWaveBins;
  bin_array_t warpWaveBins;
  bool binsReady = false;
  frange_t currentMagRange;
  int warpPoints = -1;
  int currentFrame = -1;
  float currentMaxMagnitude = 0.0f;
  float currentMedianMag = 0.04f;
  float currentZoom = 1.0f;
  bool currentHasWarp = false;
  float yPosAtFreq(const frect_t& bounds, float fNorm) const;
  fpoint_t warpPointToCanvas(const frect_t& bounds, WarpPoint* p);

public:
  FrameSpectrum(ValueTree& vt);
  void setZoomNorm(float v);
  int numWarpPoints() const { return warpPoints; }
  void setNumWarpPoints(int points);
  void frameWasFocused(int idx) override;
  void paint(juce::Graphics& g) override;
  void resized() override;
  friend class FrameWarp;

private:
  std::unique_ptr<FrameWarp> warp;
  // helpers for initializing/changing the warp points
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
  FrameWarp(FrameSpectrum* p);
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

#pragma once
#include "Electrum/Audio/Wavetable.h"
#include "Electrum/GUI/GUITypedefs.h"
#include "Electrum/GUI/WaveEditor/WaveEdiorContext.h"

#define MIN_WARP_PTS 5
#define MAX_WARP_PTS 200
#define DEFAULT_WARP_POINTS 30

#define MIN_FFT_ZOOM 1.0f
#define MAX_FFT_ZOOM 7.0f

#define FFT_GRAPH_RES AUDIBLE_BINS
class FrameSpectrum : public WaveEditListener {
  static constexpr int minWidth = FFT_GRAPH_RES;
  bin_array_t loadedWaveBins;
  bool binsReady = false;
  frange_t currentMagRange;
  int numWarpPoints = 256;
  int currentFrame = -1;
  float currentMaxMagnitude = 0.0f;
  float currentMedianMag = 0.04f;
  float currentZoom = 1.0f;
  float yPosForFreq(const frect_t& bounds, float fNorm) const;

public:
  FrameSpectrum(ValueTree& vt);
  void setZoomNorm(float v);
  void frameWasFocused(int idx) override;
  void paint(juce::Graphics& g) override;
  void resized() override;
};

class FrameSpectrumViewer : public Component {
private:
  FrameSpectrum spec;
  juce::Slider slider;
  BoundedAttString zoomStr;
  juce::Viewport vpt;

public:
  FrameSpectrumViewer(ValueTree& vt);
  void resized() override;
  void paint(juce::Graphics& g) override;
};

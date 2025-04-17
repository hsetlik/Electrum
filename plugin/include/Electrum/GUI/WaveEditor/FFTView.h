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
#define MAX_FFT_ZOOM 14.0f
#define MIN_BIN_PX 3.0f
#define MAX_CLICK_DISTANCE 6.0f
#define FFT_GRAPH_RES AUDIBLE_BINS

class FrameSpectrum : public WaveEditListener {
private:
  int currentFrame = -1;
  float currentZoom = 1.0f;

public:
  FrameSpectrum(ValueTree& vt);
  void setZoomNorm(float v);
  void frameWasFocused(int idx) override;
  void paint(juce::Graphics& g) override;
  void resized() override;
  // mouse callbacks
  void mouseDown(const juce::MouseEvent& m) override;
  void mouseUp(const juce::MouseEvent& m) override;
  void mouseDoubleClick(const juce::MouseEvent& m) override;
  void mouseDrag(const juce::MouseEvent& m) override;
};
//===============================================

//=============================================
class FrameSpectrumViewer : public Component {
private:
  FrameSpectrum spec;
  juce::Slider zoomSlider;
  BoundedAttString zoomStr;
  juce::Viewport vpt;

public:
  FrameSpectrumViewer(ValueTree& vt);
  void resized() override;
  void paint(juce::Graphics& g) override;
};

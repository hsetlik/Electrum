#include "Electrum/GUI/WaveEditor/FFTView.h"
#include "Electrum/Audio/Wavetable.h"
#include "Electrum/GUI/LookAndFeel/Color.h"
#include "Electrum/Identifiers.h"
#include "juce_core/juce_core.h"

static const float maxMag = juce::Decibels::decibelsToGain(1.0f);
static const float midMag = juce::Decibels::decibelsToGain(-18.0f);
static frange_t magRange = rangeWithCenter(0.0f, maxMag, midMag);

static float yPosForBinMagnitude(const frect_t& rect, float mag) {
  jassert(mag <= maxMag);
  auto norm = magRange.convertTo0to1(mag);
  return rect.getBottom() - (rect.getHeight() * norm);
}

static const float midBinIdx =
    (float)AUDIBLE_BINS / juce::MathConstants<float>::pi;
static frange_t binSkewRange =
    rangeWithCenter(0.0f, (float)AUDIBLE_BINS, midBinIdx);

static int binIdxForXPos(const frect_t& bounds, float xPos) {
  auto norm = (xPos - bounds.getX()) / bounds.getWidth();
  jassert(norm >= 0.0f && norm < 1.0f);
  return (int)binSkewRange.convertFrom0to1(norm);
}

//===================================================
FFTView::FFTView(ValueTree& vt) : WaveEditListener(vt) {
  auto child = waveTree.getChild(0);
  jassert(child.isValid() && child.hasType(WaveEdit::WAVE_FRAME));
  String waveStr = child[WaveEdit::frameStringData];
  Wave::loadAudibleBins(waveStr, waveBins);
}

void FFTView::frameWasFocused(int frame) {
  if (isVisible()) {
    auto child = waveTree.getChild(frame);
    jassert(child.isValid() && child.hasType(WaveEdit::WAVE_FRAME));
    String waveStr = child[WaveEdit::frameStringData];
    Wave::loadAudibleBins(waveStr, waveBins);
    repaint();
  }
}

void FFTView::paint(juce::Graphics& g) {
  auto fBounds = getLocalBounds().toFloat();
  g.setColour(UIColor::windowBkgnd);
  g.fillRect(fBounds);
  juce::Path path;
  path.startNewSubPath(0.0f, fBounds.getHeight());
  const float dX = fBounds.getWidth() / (float)numMagPoints;
  float x = 0.0f;
  float y;
  for (int i = 0; i < numMagPoints; ++i) {
    auto b = binIdxForXPos(fBounds, x);
    y = yPosForBinMagnitude(fBounds, waveBins[b].magnitude);
    path.lineTo(x, y);
    x += dX;
  }
  path.lineTo(fBounds.getWidth(), fBounds.getHeight());
  path.closeSubPath();
  g.setColour(Color::literalOrangeBright);
  g.fillPath(path);
}

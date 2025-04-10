#include "Electrum/GUI/WaveEditor/WaveThumbnail.h"
#include "Electrum/Audio/AudioUtil.h"
#include "Electrum/Audio/Wavetable.h"
#include "Electrum/GUI/LookAndFeel/Color.h"
void WaveThumbnail::updateImage() {
  // 1. parse the wave into numbers
  float temp[TABLE_SIZE];
  stringDecodeWave(waveString, temp);
  img.clear(getLocalBounds(), UIColor::windowBkgnd);
  juce::Graphics g(img);
  juce::Path p;
  static const float y0 = (float)THUMBNAIL_H / 2.0f;
  p.startNewSubPath(0.0f, y0);
  float y = y0;

  for (int i = 0; i < THUMBNAIL_W; ++i) {
    float xNorm = (float)i / (float)THUMBNAIL_W;
    int idx = (int)(xNorm * (float)TABLE_SIZE);
    y = y0 + (y0 * temp[idx]);
    p.lineTo((float)i, y);
  }
  juce::PathStrokeType pst(2.5f);
  auto strokeColor =
      drawSelected ? Color::periwinkle : Color::literalOrangePale;
  g.setColour(strokeColor);
  g.strokePath(p, pst);
  if (drawSelected) {
    auto rect = getLocalBounds().toFloat();
    g.drawRect(rect, 3.0f);
  }
}

//===================================================
WaveThumbnail::WaveThumbnail(const String& waveStr, int i)
    : img(juce::Image::RGB, THUMBNAIL_W, THUMBNAIL_H, true),
      waveString(waveStr),
      frameIndex(i) {
  updateImage();
}

void WaveThumbnail::paint(juce::Graphics& g) {
  auto fBounds = getLocalBounds().toFloat();
  g.drawImage(img, fBounds);
}


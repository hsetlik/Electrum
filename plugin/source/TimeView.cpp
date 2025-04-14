#include "Electrum/GUI/WaveEditor/TimeView.h"
#include "Electrum/Audio/AudioUtil.h"
#include "Electrum/Audio/Wavetable.h"
#include "Electrum/GUI/LookAndFeel/Color.h"
#include "Electrum/GUI/WaveEditor/EditValueTree.h"
#include "Electrum/GUI/WaveEditor/WaveEdiorContext.h"

TimeView::TimeView(ValueTree& vt) : WaveEditListener(vt) {
  frameWasFocused(0);
}

void TimeView::frameWasFocused(int idx) {
  auto child = waveTree.getChild(idx);
  jassert(child.isValid() && child.hasType(WaveEdit::WAVE_FRAME));
  String waveStr = child[WaveEdit::frameStringData];
  stringDecodeWave(waveStr, currentWave);
  repaint();
}

void TimeView::paint(juce::Graphics& g) {
  // 1. fill the background
  auto fBounds = getLocalBounds().toFloat();
  g.setColour(UIColor::windowBkgnd);
  g.fillRect(fBounds);
  // 2. draw the path
  p.clear();
  static const int viewPoints = TIME_VIEW_PTS;
  const float y0 = fBounds.getHeight() / 2.0f;
  const float yGain = y0 * 0.9f;
  p.startNewSubPath(0.0f, y0 + (currentWave[0] * yGain));
  for (int i = 1; i < viewPoints; ++i) {
    float norm = (float)i / (float)viewPoints;
    int waveIdx = (int)(norm * (float)TABLE_SIZE);
    p.lineTo(norm * fBounds.getWidth(), y0 + (currentWave[waveIdx] * yGain));
  }
  juce::PathStrokeType pst(2.3f);
  g.setColour(Color::qualifierPurple);
  g.strokePath(p, pst);
}
//===================================================

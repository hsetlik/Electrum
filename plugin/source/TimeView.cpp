#include "Electrum/GUI/WaveEditor/TimeView.h"
#include "Electrum/Audio/AudioUtil.h"
#include "Electrum/Audio/Wavetable.h"
#include "Electrum/GUI/LookAndFeel/Color.h"
#include "Electrum/GUI/WaveEditor/EditValueTree.h"
#include "Electrum/GUI/WaveEditor/WaveEdiorContext.h"
#include "Electrum/Shared/PointwiseWave.h"

TimeView::TimeView(ValueTree& vt) : WaveEditListener(vt) {
  auto child = waveTree.getChild(0);
  jassert(child.isValid() && child.hasType(WaveEdit::WAVE_FRAME));
  String waveStr = child[WaveEdit::frameStringData];
  editor.reset(new PointEditor(waveStr));
  addAndMakeVisible(editor.get());
  resized();
}

void TimeView::frameWasFocused(int idx) {
  auto child = waveTree.getChild(idx);
  jassert(child.isValid() && child.hasType(WaveEdit::WAVE_FRAME));
  const String waveStr = child[WaveEdit::frameStringData];
  editor->loadFrameString(waveStr);
  editor->toFront(true);
  resized();
}

void TimeView::resized() {
  editor->setBounds(getLocalBounds());
}

//===================================================

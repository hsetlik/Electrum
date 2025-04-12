#pragma once
#include "../Util/ModalParent.h"
#include "Electrum/GUI/WaveEditor/TimeView.h"
#include "Electrum/GUI/WaveEditor/WaveThumbnail.h"
#include "Electrum/Shared/FileSystem.h"
// an editor/viewer for one of our wavetable oscillators
class WaveEditor : public Component, public juce::TextEditor::Listener {
private:
  ElectrumState* const state;
  Wavetable* const wavetable;
  const int oscID;
  // this ValueTree holds the state of our wavetable editing
  ValueTree waveTree;
  wave_meta_t waveMeta;

  // Close/Save/So on buttons
  juce::TextButton closeBtn;
  juce::TextButton saveBtn;
  juce::TextEditor waveNameEdit;

  // Thumbnail view of our waves
  WaveThumbnailBar thumbBar;
  // time view
  std::unique_ptr<TimeView> timeView;

public:
  WaveEditor(ElectrumState* s, Wavetable* wt, int idx);
  ~WaveEditor() override;
  void resized() override;
  void textEditorTextChanged(juce::TextEditor& te) override;
  void paint(juce::Graphics& g) override;
};

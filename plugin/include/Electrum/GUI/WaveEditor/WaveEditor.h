#pragma once
#include "../Util/ModalParent.h"
#include "Electrum/GUI/WaveEditor/TimeView.h"
#include "Electrum/GUI/WaveEditor/WaveEdiorContext.h"
#include "Electrum/GUI/WaveEditor/WaveThumbnail.h"
#include "Electrum/Shared/FileSystem.h"
#include "juce_gui_basics/juce_gui_basics.h"
typedef juce::TabbedButtonBar::Orientation TabPositionE;
// tabbed middle component for our various editor views
class WaveViewerTabs : public juce::TabbedComponent {
public:
  WaveViewerTabs(ValueTree& vt);
};

//==============================================================
// an editor/viewer for one of our wavetable oscillators
class WaveEditor : public WaveEditorContext,
                   public juce::TextEditor::Listener,
                   WaveThumbnailBar::Listener {
private:
  ElectrumState* const state;
  Wavetable* const wavetable;
  const int oscID;
  // this ValueTree holds the state of our wavetable editing
  wave_meta_t waveMeta;

  // Close/Save/So on buttons
  juce::TextButton closeBtn;
  juce::TextButton saveBtn;
  juce::TextEditor waveNameEdit;

  // Thumbnail view of our waves
  std::unique_ptr<WaveThumbnailBar> thumbBar;
  // time view
  std::unique_ptr<WaveViewerTabs> tabs;

public:
  WaveEditor(ElectrumState* s, Wavetable* wt, int idx);
  ~WaveEditor() override;
  void frameWasFocused(int frame) override;
  void resized() override;
  void textEditorTextChanged(juce::TextEditor& te) override;
  void paint(juce::Graphics& g) override;
};

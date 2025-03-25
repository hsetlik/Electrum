#pragma once

#include "PluginProcessor.h"
#include "GUI/ElectrumEditor.h"
#include "juce_core/juce_core.h"

namespace audio_plugin {

class ElectrumProcessorEditor : public juce::AudioProcessorEditor {
public:
  explicit ElectrumProcessorEditor(ElectrumAudioProcessor&);
  ~ElectrumProcessorEditor() override;
  void paint(juce::Graphics&) override;
  void resized() override;

private:
  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  ElectrumAudioProcessor& processorRef;
  juce::SharedResourcePointer<juce::TooltipWindow> ttWindow;
  ElectrumEditor ed;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ElectrumProcessorEditor)
};

}  // namespace audio_plugin

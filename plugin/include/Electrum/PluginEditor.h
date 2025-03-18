#pragma once

#include "PluginProcessor.h"

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
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ElectrumProcessorEditor)
};

}  // namespace audio_plugin

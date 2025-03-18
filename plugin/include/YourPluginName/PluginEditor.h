#pragma once

#include "PluginProcessor.h"

namespace audio_plugin {

class YourPluginNameProcessorEditor : public juce::AudioProcessorEditor {
public:
  explicit YourPluginNameProcessorEditor(YourPluginNameAudioProcessor&);
  ~YourPluginNameProcessorEditor() override;
  void paint(juce::Graphics&) override;
  void resized() override;

private:
  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  YourPluginNameAudioProcessor& processorRef;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(YourPluginNameProcessorEditor)
};

}  // namespace audio_plugin

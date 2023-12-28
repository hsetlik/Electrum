

/*
  ==============================================================================
    This file contains the basic framework code for a JUCE plugin editor.
  ==============================================================================
*/

#pragma once

#include "../GUI/ElectrumEditor.h"
#include "../Parameters/ElectrumValueTree.h"
#include "PluginProcessor.h"

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800
//==============================================================================
/**
 */
class ElectrumAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
  ElectrumAudioProcessorEditor(ElectrumAudioProcessor &);
  ~ElectrumAudioProcessorEditor() override;

  //==============================================================================
  void paint(juce::Graphics &) override;
  void resized() override;

private:
  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  ElectrumAudioProcessor &audioProcessor;
  EVT *const state;
  SharedResourcePointer<TooltipWindow> tooltipWindow;
  ElectrumEditor mainPanel;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ElectrumAudioProcessorEditor)
};


 

/*
  ==============================================================================
    This file contains the basic framework code for a JUCE plugin editor.
  ==============================================================================
*/

#pragma once

#include "../Parameters/BigSynthValueTree.h"

#include "PluginProcessor.h"

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800
//==============================================================================
/**
 */
class BigSynthAudioProcessorEditor : public juce::AudioProcessorEditor {
 public:
  BigSynthAudioProcessorEditor(BigSynthAudioProcessor &);
  ~BigSynthAudioProcessorEditor() override;

  //==============================================================================
  void paint(juce::Graphics &) override;
  void resized() override;

 private:
  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  BigSynthAudioProcessor &audioProcessor;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BigSynthAudioProcessorEditor)
};
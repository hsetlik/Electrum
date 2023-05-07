
 

/*
  ==============================================================================
    This file contains the basic framework code for a JUCE plugin editor.
  ==============================================================================
*/

#include "PluginEditor.h"

#include "PluginProcessor.h"

//==============================================================================
BigSynthAudioProcessorEditor::BigSynthAudioProcessorEditor(
    BigSynthAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p) {
  // Make sure that before the constructor has finished, you've set the
  // editor's size to whatever you need it to be.
  setSize (WINDOW_WIDTH, WINDOW_HEIGHT);
  // load Image from BinaryData
  
}

BigSynthAudioProcessorEditor::~BigSynthAudioProcessorEditor() {}

//==============================================================================
void BigSynthAudioProcessorEditor::paint(juce::Graphics &g) 
{
  // (Our component is opaque, so we must completely fill the background with a
  // solid colour)
  g.fillAll(
      getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
  g.setColour(juce::Colours::black);
  g.setFont(30.0f);
  g.drawFittedText("Hello World!", getLocalBounds(),
                   juce::Justification::centred, 1);
}

void BigSynthAudioProcessorEditor::resized() 
{
  // This is generally where you'll want to lay out the positions of any
  // subcomponents in your editor..
}
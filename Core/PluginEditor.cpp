
 

/*
  ==============================================================================
    This file contains the basic framework code for a JUCE plugin editor.
  ==============================================================================
*/

#include "PluginEditor.h"

#include "PluginProcessor.h"

//==============================================================================
ElectrumAudioProcessorEditor::ElectrumAudioProcessorEditor(ElectrumAudioProcessor &p) :
 AudioProcessorEditor(&p), 
 audioProcessor(p),
 state(&p.state),
 mainPanel(state)
{
  // Make sure that before the constructor has finished, you've set the
  // editor's size to whatever you need it to be.
  addAndMakeVisible(&mainPanel);
  setSize (WINDOW_WIDTH, WINDOW_HEIGHT);
}

ElectrumAudioProcessorEditor::~ElectrumAudioProcessorEditor() {}

//==============================================================================
void ElectrumAudioProcessorEditor::paint(juce::Graphics &g) 
{

}

void ElectrumAudioProcessorEditor::resized() 
{
  mainPanel.setBounds(getLocalBounds());
}
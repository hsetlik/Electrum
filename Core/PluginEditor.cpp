/*
  ==============================================================================
    This file contains the basic framework code for a JUCE plugin editor.
  ==============================================================================
*/

#include "PluginEditor.h"
#include "PluginProcessor.h"

//==============================================================================
ElectrumAudioProcessorEditor::ElectrumAudioProcessorEditor(ElectrumAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p), state(&p.state), mainPanel(state)
{
  // Make sure that before the constructor has finished, you've set the
  // editor's size to whatever you need it to be.
  addAndMakeVisible(&mainPanel);
  setSize(WINDOW_WIDTH, WINDOW_HEIGHT);
  // let the graphics code know it needs to do stuff now
  state->editorOpened();
}

ElectrumAudioProcessorEditor::~ElectrumAudioProcessorEditor() { state->editorClosed(); }

//==============================================================================
void ElectrumAudioProcessorEditor::paint(juce::Graphics &)
{
  //  g.fillAll(Color::dimGray);
}

void ElectrumAudioProcessorEditor::resized() { mainPanel.setBounds(getLocalBounds()); }

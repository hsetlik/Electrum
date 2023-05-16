#include "ElectrumEditor.h"

ElectrumEditor::ElectrumEditor(EVT* tree) : state(tree)
{
    for (int i = 0; i < NUM_OSCILLATORS; ++i)
    {
        oscEditors.add(new OscillatorEditor(state, i));
        addAndMakeVisible(oscEditors.getLast());
    }

}
ElectrumEditor::~ElectrumEditor()
{

}

void ElectrumEditor::paint(Graphics& g) 
{

}
void ElectrumEditor::resized() 
{
    auto lBounds = getLocalBounds();
    auto oscArea = lBounds.removeFromTop((int)((float)lBounds.getHeight() * 0.35));
    auto oscWidth = oscArea.getWidth() / (NUM_OSCILLATORS + 1);
    DLog::log("Oscillator width: " + String(oscWidth));
    for (int i = 0; i < NUM_OSCILLATORS; i++)
    {
        auto a = oscArea.removeFromLeft(oscWidth);
        oscEditors[i]->setBounds(a);
    }

}
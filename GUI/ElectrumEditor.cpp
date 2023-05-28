#include "ElectrumEditor.h"

ElectrumEditor::ElectrumEditor(EVT* tree) : 
state(tree), 
modWhlSource(tree),
pitchWhlSource(tree),
perlin(tree)
{
    setLookAndFeel(&lnf);
    for (int i = 0; i < NUM_OSCILLATORS; ++i)
    {
        oscEditors.add(new OscillatorEditor(state, i));
        addAndMakeVisible(oscEditors.getLast());
    }
    addAndMakeVisible(modWhlSource);
    addAndMakeVisible(pitchWhlSource);
    addAndMakeVisible(perlin);

}
ElectrumEditor::~ElectrumEditor()
{
    setLookAndFeel(nullptr);

}

void ElectrumEditor::paint(Graphics& ) 
{

}
void ElectrumEditor::resized() 
{
    auto lBounds = getLocalBounds().toFloat();
    auto modArea = lBounds.removeFromLeft(lBounds.getWidth() / 5.0f);
    modWhlSource.setBounds(modArea.removeFromTop(modArea.getWidth() / 3.0f).toNearestInt());
    pitchWhlSource.setBounds(modArea.removeFromTop(modArea.getWidth() / 3.0f).toNearestInt());
    perlin.setBounds(modArea.removeFromTop(modArea.getWidth()).toNearestInt());
    auto oscArea = lBounds.removeFromTop(lBounds.getHeight() * 0.35f);
    auto oscWidth = oscArea.getWidth() / (NUM_OSCILLATORS + 1);
    for (int i = 0; i < NUM_OSCILLATORS; i++)
    {
        auto a = oscArea.removeFromLeft(oscWidth).toNearestInt();
        oscEditors[i]->setBounds(a);
    }

}
#include "OscillatorEditor.h"

OscillatorEditor::OscillatorEditor(EVT* tree, int idx) :
state(tree),
index(idx),
label("oscLabel" + String(idx), "Oscillator " + String(idx)),
sLevel(tree, IDs::oscillatorLevel.toString() + String(idx)),
sPos(tree, IDs::oscillatorPos.toString() + String(idx))
{

    addAndMakeVisible(&label);
    addAndMakeVisible(&sLevel);
    addAndMakeVisible(&sPos);

}

void OscillatorEditor::resized()
{
    auto lBounds = getLocalBounds().toFloat();
    auto labelRegion = lBounds.removeFromTop(18.0f);
    label.setBounds(labelRegion.reduced(2.0f).toNearestInt());
    auto knobsRegion = lBounds.removeFromTop(lBounds.getHeight() * 0.35f);
    sLevel.setBounds(knobsRegion.removeFromLeft(knobsRegion.getHeight()).toNearestInt());
    sPos.setBounds(knobsRegion.removeFromLeft(knobsRegion.getHeight()).toNearestInt());


}
    
void OscillatorEditor::paint(Graphics& g) 
{
    g.fillAll(Colour::fromRGB(40, 60, 92));
    
}
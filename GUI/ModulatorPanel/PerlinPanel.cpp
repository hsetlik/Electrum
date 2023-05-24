#include "PerlinPanel.h"
PerlinPanel::PerlinPanel(EVT* tree) : 
state(tree),
sFreq(Slider::Rotary, Slider::NoTextBox),
sLac(Slider::Rotary, Slider::NoTextBox),
sOct(Slider::Rotary, Slider::NoTextBox)
{
    startTimerHz(METER_REFRESH_HZ);
    freqAttach.reset(new sAttach(*state->getAPVTS(), IDs::perlinFreq.toString(), sFreq));
    lacAttach.reset(new sAttach(*state->getAPVTS(), IDs::perlinLacunarity.toString(), sLac));
    octAttach.reset(new sAttach(*state->getAPVTS(), IDs::perlinOctaves.toString(), sOct));

    addAndMakeVisible(&sFreq);
    addAndMakeVisible(&sLac);
    addAndMakeVisible(&sOct);
}
void PerlinPanel::resized() 
{
    auto lBounds = getLocalBounds().toFloat();
    auto paramArea = lBounds.removeFromRight(lBounds.getWidth() * (1.0f - METER_WIDTH));
    auto pWidth = paramArea.getWidth() / 4.0f;
    sFreq.setBounds(paramArea.removeFromLeft(pWidth).toNearestInt());
    sLac.setBounds(paramArea.removeFromLeft(pWidth).toNearestInt());
    sOct.setBounds(paramArea.removeFromLeft(pWidth).toNearestInt());
}
void PerlinPanel::paint(Graphics& g) 
{
    static int numRepaints = 0;
    auto lBounds = getLocalBounds().toFloat();
    auto meterBounds = lBounds.removeFromRight(lBounds.getWidth() * METER_WIDTH);
    g.setColour(Color::darkGray);
    g.fillRect(meterBounds);
    g.setColour(Color::lightTeal);
    const float perlinVal = state->perlinValue();
    const float centerY =  meterBounds.getY() + (meterBounds.getHeight() / 2.0f);
    const float rectHeight = perlinVal * meterBounds.getHeight();

    g.fillRect(Rectangle<float>(meterBounds.getX(), meterBounds.getBottom() - rectHeight, meterBounds.getWidth(), rectHeight));
    ++numRepaints;
    if (numRepaints == 15)
    {
        numRepaints = 0;
        DLog::log("Perlin value: " + String(perlinVal));
    }

}
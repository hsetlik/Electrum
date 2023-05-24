#include "DepthSlider.h"

void DepthSliderLookAndFeel::drawRotarySlider (Graphics & g, 
int x, 
int y, 
int width, 
int height, 
float sliderPosProportional, 
float rotaryStartAngle, 
float rotaryEndAngle, 
Slider &) 
{
    TRACE_COMPONENT();
    //step 1: Draw the main arc
    Rectangle<int> iArea(x, y, width, height);
    auto fArea = iArea.toFloat();
    g.setColour(Color::lightTeal);
    auto sliderArea = fArea.reduced(5.0f);
    GUIUtil::strokeArc(g, sliderArea, rotaryStartAngle, rotaryEndAngle, 2.0f);
    auto toAngle = Math::flerp(rotaryStartAngle, rotaryEndAngle, sliderPosProportional);
    //step 2: Draw the thumb
    auto radius = (fArea.getWidth() / 2.0f) * 0.95f;
    Point<float> thumbPoint (fArea.getCentreX() + radius * std::cos (toAngle - MathConstants<float>::halfPi),
                             fArea.getCentreY() + radius * std::sin (toAngle - MathConstants<float>::halfPi));
    const float thumbWidth = 9.0f;
    g.setColour(Color::lightGray);
    g.fillEllipse (Rectangle<float> (thumbWidth, thumbWidth).withCentre (thumbPoint));
}
//=============================================================================================
void CloseButton::paintButton(Graphics& g, bool highlighted, bool down) 
{
    auto lBounds = getLocalBounds().toFloat();
    if (highlighted || down)
    {
        g.setColour(Color::closeRedBright);
    }
    else
    {
        g.setColour(Color::closeRedDark);
    }
    g.fillEllipse(lBounds);
    g.setColour(Color::darkGray);
    g.fillPath(xPath);
}
//=============================================================================================
DepthSlider::DepthSlider(EVT* tree, const String& src, const String& dst, int idx) :
state(tree),
sourceID(src),
destID(dst),
destIndex(idx),
slider(Slider::SliderStyle::Rotary, Slider::TextEntryBoxPosition::NoTextBox),
lastSliderValue(0.0f)
{
    setLookAndFeel(&lnf);
    addAndMakeVisible(&slider);
    slider.addListener(this);
    slider.setRange(-1.0f, 1.0f, 0.0001f);
    slider.setValue(0.0f);
    slider.setRotaryParameters(getDepthSliderParams());
    state->setModulation(sourceID, destID, lastSliderValue);
}

DepthSlider::~DepthSlider()
{
    setLookAndFeel(nullptr);
    state->removeModulation(sourceID, destID);
    slider.removeListener(this);
}

void DepthSlider::sliderValueChanged(Slider* s) 
{
    float newVal = (float)s->getValue();
    if (newVal != lastSliderValue)
    {
        lastSliderValue = newVal;
        state->setModulation(sourceID, destID, lastSliderValue);
    }

}

void DepthSlider::resized()
{
    slider.setBounds(getLocalBounds());
}
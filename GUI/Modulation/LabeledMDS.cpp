#include "FontBinaries.h"
#include "LabeledMDS.h"

LabeledMDS::LabeledMDS(EVT *tree, const String &id, const String &labelText)
    : mds(std::make_unique<ModulationDestSlider>(tree, id)), text(labelText),
      labelFont(Typeface::createSystemTypefaceFor(
          FontBinaries::RobotoMediumItalic_ttf,
          FontBinaries::RobotoMediumItalic_ttfSize)),
      textColor(Color::black)
{
  addAndMakeVisible(mds.get());
}

void LabeledMDS::resized()
{
  float dX = (float)getWidth() / 16.0f;
  Rectangle<float> sliderBounds(2.0f * dX, dX, 12.0f * dX, 12.0f * dX);
  mds->setBounds(sliderBounds.toNearestInt());
}

void LabeledMDS::paint(Graphics &g)
{
  float dX = (float)getWidth() / 16.0f;
  Rectangle<float> labelLimits(0.0f, 13.0f * dX, 16.0f * dX, 3.0f * dX);
  g.setFont(labelFont);
  g.setColour(textColor);
  g.drawFittedText(text, labelLimits.toNearestInt(), Justification::centred, 2);
}

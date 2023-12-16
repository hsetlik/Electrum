#include "FontBinaries.h"
#include "LabeledMDS.h"

LabeledMDS::LabeledMDS(EVT *tree, const String &id, const String &labelText)
    : mds(std::make_unique<ModulationDestSlider>(tree, id)), text(labelText),
      textColor(Color::black)
{
  label.setText(text, juce::dontSendNotification);
  label.setJustificationType(Justification::centred);
  label.setBorderSize(BorderSize<int>(3));
  addAndMakeVisible(&label);
  addAndMakeVisible(mds.get());
}

void LabeledMDS::resized()
{
  float dX = (float)getWidth() / 16.0f;
  Rectangle<float> sliderBounds(2.0f * dX, dX, 12.0f * dX, 12.0f * dX);
  mds->setBounds(sliderBounds.toNearestInt());
  Rectangle<float> labelLimits(0.0f, 13.0f * dX, 16.0f * dX, 3.0f * dX);
  label.setBounds(labelLimits.toNearestInt());
}

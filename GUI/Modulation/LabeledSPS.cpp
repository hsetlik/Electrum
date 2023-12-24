#include "LabeledSPS.h"

LabeledSPS::LabeledSPS(EVT *tree, const String &param, const String &labelText, bool vertical)
    : text(labelText), isVertical(vertical),
      sps(std::make_unique<SourceParamSlider>(tree, param, vertical))
{
  label.setJustificationType(Justification::centred);
  label.setBorderSize(BorderSize<int>(1));
  label.setText(text, juce::dontSendNotification);
  addAndMakeVisible(label);
  addAndMakeVisible(sps.get());
}

LabeledSPS::~LabeledSPS() {}

void LabeledSPS::resized()
{
  auto fBounds = getLocalBounds().toFloat();
  float dX = fBounds.getWidth() / 16.0f;
  if (isVertical)
  {
    float labelHeight = 4.0f * dX;
    float sliderHeight = fBounds.getHeight() - (labelHeight + dX);
    Rectangle<float> sliderBounds(dX, dX, 14.0f * dX, sliderHeight);
    Rectangle<float> labelBounds(0.0f, sliderHeight + dX, 16.0f * dX, labelHeight);
    sps->setBounds(sliderBounds.toNearestInt());
    label.setBounds(labelBounds.toNearestInt());
  } else
  {
    Rectangle<float> sliderBounds(2.0f * dX, 0.0f, 12.0f * dX, 12.0f * dX);
    Rectangle<float> labelBounds(0.0f, 12.0f * dX, 16.0f * dX, 4.0f * dX);
    sps->setBounds(sliderBounds.toNearestInt());
    label.setBounds(labelBounds.toNearestInt());
  }
}

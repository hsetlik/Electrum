#include "Electrum/GUI/WaveEditor/PointEditor.h"
#include "Electrum/Audio/AudioUtil.h"
#include "Electrum/Audio/Wavetable.h"
#include "juce_audio_basics/juce_audio_basics.h"

ViewedPointEditor::ViewedPointEditor(const String& frameStr)
    : frame(std::make_unique<Pointwise::Warp>(frameStr)) {
  startTimerHz(24);
}

void ViewedPointEditor::timerCallback() {
  repaint();
}

void ViewedPointEditor::resized() {
  auto* vpt = findParentComponentOfClass<juce::Viewport>();
  if (vpt != nullptr) {
    int width = getLocalBounds().getWidth();
    const float height = (float)vpt->getMaximumVisibleHeight();
    const int viewWidth = vpt->getMaximumVisibleWidth();
    // make sure we can't zoom out so far that part of the viewport is empty
    if (viewWidth > width)
      width = viewWidth;
    setSize(width, (int)(height * 0.9f));
  }
}

void ViewedPointEditor::setWidthForScale(float normScale) {
  static frange_t scaleRange = rangeWithCenter(0.3f, 5.0f, 1.0f);
  const float fScale = scaleRange.convertFrom0to1(normScale);
  auto* vpt = findParentComponentOfClass<juce::Viewport>();
  if (vpt != nullptr) {
    int width = (int)(fScale * (float)TABLE_SIZE);
    const float height = (float)vpt->getMaximumVisibleHeight();
    const int viewWidth = vpt->getMaximumVisibleWidth();
    // make sure we can't zoom out so far that part of the viewport is empty
    if (viewWidth > width)
      width = viewWidth;
    setSize(width, (int)(height * 0.9f));
  }
}

void ViewedPointEditor::paint(juce::Graphics& g) {
  auto fBounds = getLocalBounds().toFloat();
  auto* vpt = findParentComponentOfClass<juce::Viewport>();
  jassert(vpt != nullptr);
  auto viewedBounds = vpt->getViewArea().toFloat();
  const float normStart = viewedBounds.getX() / fBounds.getWidth();
  const float normEnd = viewedBounds.getRight() / fBounds.getWidth();
  frame->drawSection(g, fBounds, normStart, normEnd);
}
//===================================================

PointEditor::PointEditor(const String& frameStr) : editor(frameStr) {
  vpt.setViewedComponent(&editor, false);
  vpt.setViewPosition(0, 0);
  vpt.setInterceptsMouseClicks(true, true);
  vpt.setRepaintsOnMouseActivity(false);
  addAndMakeVisible(vpt);
  editor.resized();

  scaleSlider.setSliderStyle(juce::Slider::LinearBarVertical);
  scaleSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 1, 1);
  scaleSlider.setRange(0.0, 1.0);
  scaleSlider.setValue(0.5);
  addAndMakeVisible(scaleSlider);
  scaleSlider.onValueChange = [this] {
    const float norm = (float)scaleSlider.getValue();
    editor.setWidthForScale(norm);
  };
}

PointEditor::~PointEditor() {
  auto* parent = getParentComponent();
  if (parent != nullptr) {
    parent->removeChildComponent(this);
  }
}

void PointEditor::loadFrameString(const String& str) {
  normScale = 0.5f;
  scaleSlider.setValue(0.5);
  vpt.setViewPosition(0, 0);
  editor.loadFrameString(str);
}

void PointEditor::resized() {
  auto fBounds = getLocalBounds().toFloat();
  static const float sliderWidth = 10.0f;
  auto sBounds = fBounds.removeFromLeft(sliderWidth);
  scaleSlider.setBounds(sBounds.toNearestInt());
  vpt.setBounds(fBounds.toNearestInt());
}

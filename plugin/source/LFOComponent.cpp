#include "Electrum/GUI/ModulatorPanel/LFOComponent.h"
#include "Electrum/Audio/Modulator/LFO.h"
#include "Electrum/GUI/GUITypedefs.h"
#include "Electrum/GUI/LookAndFeel/Color.h"
#include "Electrum/GUI/LookAndFeel/Fonts.h"
#include "Electrum/Identifiers.h"
LFOThumbnail::LFOThumbnail(ElectrumState* s, int idx) : state(s), lfoID(idx) {
  shapeStringID = ID::lfoShapeString.toString() + String(lfoID);
  loadShapePoints();
}

void LFOThumbnail::loadShapePoints() {
  auto lfoTree = state->state.getChildWithName(ID::LFO_INFO);
  jassert(lfoTree.isValid());
  String _shapeStr = lfoTree[shapeStringID];
  std::vector<lfo_handle_t> handles = {};
  LFO::stringDecode(_shapeStr, handles);
  shapePts.clear();
  for (auto& h : handles) {
    float x = (float)h.tableIdx / (float)(LFO_SIZE - 1);
    shapePts.push_back({x, h.level});
  }
}

void LFOThumbnail::graphingDataUpdated(GraphingData* gd) {
  const float _phase = gd->getLFOPhase(lfoID);
  if (!fequal(_phase, currentPhase)) {
    currentPhase = _phase;
    repaint();
  }
}

static juce::Path s_generateLfoPath(const frect_t& bounds,
                                    const std::vector<fpoint_t>& points) {
  juce::Path p;
  p.startNewSubPath(bounds.getBottomLeft());
  for (auto& point : points) {
    const float xPos = bounds.getX() + (bounds.getWidth() * point.x);
    const float yPos = bounds.getBottom() - (bounds.getHeight() * point.y);
    p.lineTo(xPos, yPos);
  }
  p.lineTo(bounds.getBottomRight());
  return p;
}

void LFOThumbnail::paint(juce::Graphics& g) {
  // 1. fill the background
  auto fBounds = getLocalBounds().toFloat();
  g.setColour(UIColor::windowBkgnd);
  g.fillRect(fBounds);
  fBounds = fBounds.reduced(2.0f);
  // 2. draw the outline
  auto color =
      isEnabled() ? Color::qualifierPurple : Color::qualifierPurple.darker();
  g.setColour(color);
  static const float strokeWidth = 2.0f;
  juce::PathStrokeType pst(strokeWidth);
  g.drawRect(fBounds, strokeWidth);

  // 3. draw the LFO shape
  auto wavePath = s_generateLfoPath(fBounds, shapePts);
  g.strokePath(wavePath, pst);

  // 4. draw the leading voice position
  if (isEnabled()) {
    juce::Path leadPath;
    const float xPos = fBounds.getX() + (fBounds.getWidth() * currentPhase);
    leadPath.startNewSubPath(xPos, fBounds.getY());
    leadPath.lineTo(xPos, fBounds.getBottom());
    g.strokePath(leadPath, pst);
  }
}
//===================================================

LFOComponent::LFOComponent(ElectrumState* s, int idx) : state(s), lfoID(idx) {
  setLookAndFeel(&lnf);
  // 1. set up the freq slider/attachment
  freqSlider.setSliderStyle(juce::Slider::Rotary);
  freqSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 45, 12);
  const String freqParamID = ID::lfoFrequency.toString() + String(lfoID);
  freqAttach.reset(
      new apvts::SliderAttachment(*state, freqParamID, freqSlider));
  addAndMakeVisible(freqSlider);
  // 2. set up the trigger mode selector/attachment
  trigModeCB.addItemList(getTriggerModeNames(), 1);
  trigModeCB.setSelectedItemIndex(0);
  const String trigParamID = ID::lfoTriggerMode.toString() + String(lfoID);
  trigModeAttach.reset(
      new apvts::ComboBoxAttachment(*state, trigParamID, trigModeCB));
  addAndMakeVisible(trigModeCB);
  // 3. set up the thumbnail
  thumb.reset(new LFOThumbnail(state, lfoID));
  addAndMakeVisible(thumb.get());
  // 4. set up the labels
  auto bigFont = FontData::getFontWithHeight(FontE::RobotoMI, 18.0f);
  auto smallFont = FontData::getFontWithHeight(FontE::RobotoMI, 14.0f);
  nameLabel.aString.setFont(bigFont);
  nameLabel.aString.setText("LFO " + String(lfoID + 1));
  nameLabel.aString.setColour(UIColor::defaultText);
  nameLabel.aString.setJustification(juce::Justification::centred);
  freqLabel.aString.setFont(smallFont);
  freqLabel.aString.setText("Frequency");
  freqLabel.aString.setColour(UIColor::defaultText);
  freqLabel.aString.setJustification(juce::Justification::centred);
  trigModeLabel.aString.setFont(smallFont);
  trigModeLabel.aString.setText("Retrigger");
  trigModeLabel.aString.setColour(UIColor::defaultText);
  trigModeLabel.aString.setJustification(juce::Justification::centred);
  // 5. start the timer
  startTimerHz(LFO_STRING_CHECK_HZ);
}

LFOComponent::~LFOComponent() {
  setLookAndFeel(nullptr);
}

void LFOComponent::timerCallback() {
  if (isVisible()) {
    const String lfoStringID = ID::lfoShapeString + String(lfoID);
    auto lfoTree = state->state.getChildWithName(ID::LFO_INFO);
    jassert(lfoTree.isValid());
    String shapeStr = lfoTree[lfoStringID];
    size_t newHash = shapeStr.hash();
    if (newHash != lastLfoHash) {
      lastLfoHash = newHash;
      thumb.reset(new LFOThumbnail(state, lfoID));
      // addAndMakeVisible(thumb.get());
      repaint();
    }
  }
}

void LFOComponent::resized() {
  auto fBounds = getLocalBounds().toFloat();
  const float topRowHeight = std::min(fBounds.getHeight() / 4.0f, 22.0f);
  auto topRowBounds = fBounds.removeFromTop(topRowHeight);
  // 1. place the name label
  nameLabel.bounds = topRowBounds.removeFromLeft(fBounds.getWidth() * 0.75f);
  // 2. place the thumbnail
  auto thumbBounds = fBounds.removeFromRight(fBounds.getWidth() * 0.6f);
  thumb->setBounds(thumbBounds.toNearestInt());
  // 3. place the trigger mode controls
  const float trigHeight = std::max(32.0f, fBounds.getHeight() / 3.3f);
  trigModeLabel.bounds = fBounds.removeFromTop(trigHeight / 2.0f);
  auto tBoxBounds = fBounds.removeFromTop(trigHeight / 2.0f);
  trigModeCB.setBounds(tBoxBounds.reduced(3.0f).toNearestInt());
  freqLabel.bounds = fBounds.removeFromTop(trigHeight / 2.0f);
  freqSlider.setBounds(fBounds.reduced(8.0f).toNearestInt());
}

void LFOComponent::paint(juce::Graphics& g) {
  auto fBounds = getLocalBounds().toFloat();
  // 1. outline/background
  g.setColour(UIColor::widgetBkgnd);
  g.fillRect(fBounds);
  g.setColour(UIColor::defaultText);
  g.drawRect(fBounds, 2.5f);
  // 2. labels
  nameLabel.draw(g);
  freqLabel.draw(g);
  trigModeLabel.draw(g);
}

//---------------------------------------------------------
LFOTabs::LFOTabs(ElectrumState* s)
    : juce::TabbedComponent(TabBar::Orientation::TabsAtLeft) {
  for (int i = 0; i < NUM_LFOS; ++i) {
    String name = "LFO " + String(i + 1);
    addTab(name, UIColor::widgetBkgnd, new LFOComponent(s, i), true);
  }
  resized();
}


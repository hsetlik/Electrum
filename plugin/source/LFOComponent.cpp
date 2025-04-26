#include "Electrum/GUI/ModulatorPanel/LFOComponent.h"
#include "Electrum/Audio/Modulator/LFO.h"
#include "Electrum/GUI/GUITypedefs.h"
#include "Electrum/GUI/LookAndFeel/Color.h"
#include "Electrum/GUI/LookAndFeel/Fonts.h"
#include "Electrum/GUI/Modulation/ModSourceButton.h"
#include "Electrum/GUI/Util/ModalParent.h"
#include "Electrum/Identifiers.h"
#include "Electrum/Shared/ElectrumState.h"
#include "juce_audio_processors/juce_audio_processors.h"
#include "juce_core/juce_core.h"
#include "juce_events/juce_events.h"
#include "juce_gui_basics/juce_gui_basics.h"
LFOThumbnail::LFOThumbnail(ElectrumState* s, int idx) : state(s), lfoID(idx) {
  shapeStringID = ID::lfoShapeString.toString() + String(lfoID);
  loadShapePoints();
  // attach itself as a listener
  state->graph.addListener(this);
}

LFOThumbnail::~LFOThumbnail() {
  auto* parent = getParentComponent();
  if (parent != nullptr) {
    parent->removeChildComponent(this);
  }
  state->graph.removeListener(this);
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
    triggerAsyncUpdate();
  }
}

void LFOThumbnail::handleAsyncUpdate() {
  // TODO: we also need to load the new shape if needed
  repaint();
}

void LFOThumbnail::mouseDoubleClick(const juce::MouseEvent& me) {
  juce::ignoreUnused(me);
  ModalParent::openLFOEditor(this, state, lfoID);
}

void LFOThumbnail::resized() {
  repaint();
}

static juce::Path s_generateLfoPath(const frect_t& bounds,
                                    const std::vector<fpoint_t>& points) {
  juce::Path p;
  p.startNewSubPath(bounds.getBottomLeft());
  const float waveMaxY = bounds.getBottom() - 3.5f;
  const float waveMaxH = (bounds.getHeight() - 3.5f) * 0.9f;
  const float y0 = waveMaxY - (points[0].y * waveMaxH);
  for (auto& point : points) {
    const float xPos = bounds.getX() + (bounds.getWidth() * point.x);
    const float yPos = waveMaxY - (point.y * waveMaxH);
    p.lineTo(xPos, yPos);
  }
  p.lineTo(bounds.getRight(), y0);
  p.lineTo(bounds.getBottomRight());
  return p;
}

static frect_t s_makeThumbnailBounds(const frect_t& input) {
  const float width = input.getWidth();
  const float height = width * 0.85f;
  return input.withSizeKeepingCentre(width, height);
}

void LFOThumbnail::paint(juce::Graphics& g) {
  // 1. fill the background
  auto fBounds = getLocalBounds().toFloat();
  g.setColour(UIColor::menuBkgnd);
  fBounds = s_makeThumbnailBounds(fBounds);
  g.fillRect(fBounds);
  fBounds = fBounds.reduced(2.0f);
  // 2. draw the outline
  auto color =
      isEnabled() ? Color::qualifierPurple : Color::qualifierPurple.darker();
  g.setColour(color);
  static const float strokeWidth = 1.7f;
  juce::PathStrokeType pst(strokeWidth);
  g.drawRect(fBounds, strokeWidth);

  // 3. draw the LFO shape
  auto wavePath = s_generateLfoPath(fBounds, shapePts);
  g.strokePath(wavePath, pst);

  // 4. draw the leading voice position
  if (isEnabled() && !fequal(currentPhase, 0.0f)) {
    juce::Path leadPath;
    const float xPos = fBounds.getX() + (fBounds.getWidth() * currentPhase);
    leadPath.startNewSubPath(xPos, fBounds.getY());
    leadPath.lineTo(xPos, fBounds.getBottom());
    g.strokePath(leadPath, pst);
  }
}
//===================================================

SubdivSlider::SubdivSlider() {
  setSliderStyle(juce::Slider::Rotary);
  setTextBoxStyle(juce::Slider::TextBoxBelow, true, 45, 12);
  juce::NormalisableRange<double> sRange(0.0, 9.0);
  setNormalisableRange(sRange);
}

double SubdivSlider::snapValue(double attemptedValue, juce::Slider::DragMode) {
  const int nearest = (int)attemptedValue;
  return (double)std::min(nearest, NUM_SUBDIVS - 1);
}

String SubdivSlider::getTextFromValue(double value) {
  auto names = getNoteSubdivNames();
  return names[(int)value];
}
//===================================================

void LFOComponent::subAttachCallback(float val) {
  subSlider.setValue(val, juce::dontSendNotification);
}

LFOComponent::LFOComponent(ElectrumState* s, int idx) : state(s), lfoID(idx) {
  // 1. set up the freq slider/attachment
  hzSlider.setSliderStyle(juce::Slider::Rotary);
  hzSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 45, 12);
  const String freqParamID = ID::lfoFrequencyHz.toString() + String(lfoID);
  hzAttach.reset(new apvts::SliderAttachment(*state, freqParamID, hzSlider));
  addAndMakeVisible(hzSlider);

  // 2. deal with the subdivision slider/ custom attachment
  addAndMakeVisible(subSlider);
  subSlider.setVisible(false);
  const String subdivParamID = ID::lfoNoteSubdiv.toString() + String(lfoID);
  auto* subdivParam = state->getParameter(subdivParamID);
  jassert(subdivParam != nullptr);
  auto subdivCallback = [this](float value) { subAttachCallback(value); };
  subAttach.reset(new juce::ParameterAttachment(*subdivParam, subdivCallback,
                                                state->undoManager));
  subAttach->sendInitialUpdate();
  // and a lambda fo handling the subSlider
  subSlider.onValueChange = [this]() {
    subAttach->setValueAsCompleteGesture((float)subSlider.getValue());
  };

  // 3. set up the BPM/Hz toggle button
  addAndMakeVisible(syncBtn);
  syncBtn.setClickingTogglesState(true);
  const String syncParamID = ID::lfoBeatSync.toString() + String(lfoID);
  syncAttach.reset(new apvts::ButtonAttachment(*state, syncParamID, syncBtn));
  // toggle the visibility of the sliders
  syncBtn.onClick = [this]() {
    const bool subVisible = syncBtn.getToggleState();
    hzSlider.setVisible(!subVisible);
    hzSlider.setEnabled(!subVisible);
    subSlider.setVisible(subVisible);
    subSlider.setEnabled(subVisible);
    repaint();
  };

  // 2. set up the trigger mode selector/attachment
  trigModeCB.addItemList(getTriggerModeNames(), 1);
  trigModeCB.setSelectedItemIndex(0);
  addAndMakeVisible(trigModeCB);
  const String trigParamID = ID::lfoTriggerMode.toString() + String(lfoID);
  trigModeAttach.reset(
      new apvts::ComboBoxAttachment(*state, trigParamID, trigModeCB));
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

LFOComponent::~LFOComponent() {}

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
      addAndMakeVisible(thumb.get());
      resized();
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
  thumbBounds =
      fBounds.removeFromRight(fBounds.getWidth() * 0.6f).reduced(3.0f);
  thumb->setBounds(thumbBounds.toNearestInt());
  jassert(thumb->getParentComponent() != nullptr);
  thumb->toFront(false);
  // 3. place the trigger mode controls
  const float trigHeight = std::max(32.0f, fBounds.getHeight() / 3.3f);
  trigModeLabel.bounds = fBounds.removeFromTop(trigHeight / 2.0f);
  auto tBoxBounds = fBounds.removeFromTop(trigHeight / 2.0f);
  trigModeCB.setBounds(tBoxBounds.reduced(3.5f).toNearestInt());
  freqLabel.bounds = fBounds.removeFromTop(trigHeight / 2.0f);
  auto syncBounds = fBounds.removeFromRight(35.0f);
  syncBtn.setBounds(syncBounds.removeFromTop(18.0f).toNearestInt());
  auto sBounds = fBounds.reduced(8.0f).toNearestInt();
  hzSlider.setBounds(sBounds);
  subSlider.setBounds(sBounds);
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
LFOTabs::LFOTabs(ElectrumState* s) {
  for (int i = 0; i < NUM_LFOS; ++i) {
    // 1. add the content panels
    auto* lfo = lfos.add(new LFOComponent(s, i));
    addAndMakeVisible(lfo);
    // 2. add the buttons
    String name = "LFO " + String(i + 1);
    const int srcID = (int)(ModSourceE::LFO1) + i;
    auto* btn = buttons.add(new ModSourceButton(s, srcID, name));
    btn->onClick = [this, i]() { setSelected(i); };
    addAndMakeVisible(btn);
  }
  buttons.getLast()->setSelected(true);
}

void LFOTabs::setSelected(int idx) {
  if (idx != selectedLfo) {
    selectedLfo = idx;
    for (int i = 0; i < NUM_LFOS; ++i) {
      const bool sel = i == selectedLfo;
      buttons[i]->setSelected(sel);
      lfos[i]->setVisible(sel);
      lfos[i]->setEnabled(sel);
    }
  }
  resized();
}

void LFOTabs::resized() {
  auto fBounds = getLocalBounds().toFloat();
  static constexpr float tabWidth = 85.0f;
  static constexpr float maxTabHeight = 37.0f;
  const float tabHeight =
      std::min(fBounds.getHeight() / (float)NUM_LFOS, maxTabHeight);
  auto tabBounds = fBounds.removeFromLeft(tabWidth);
  for (int i = 0; i < lfos.size(); ++i) {
    lfos[i]->setBounds(fBounds.toNearestInt());
    auto tBounds = tabBounds.removeFromTop(tabHeight);
    buttons[i]->setBounds(tBounds.toNearestInt());
  }
}

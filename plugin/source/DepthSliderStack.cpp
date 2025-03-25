#include "Electrum/GUI/Modulation/DepthSliderStack.h"
#include "Electrum/Common.h"
#include "Electrum/GUI/LookAndFeel/Color.h"
#include "Electrum/Identifiers.h"
#include "juce_events/juce_events.h"
#include "juce_graphics/juce_graphics.h"
#include "juce_gui_basics/juce_gui_basics.h"

// Depth Sliders ====================================
static void _strokeArc(juce::Graphics& g,
                       juce::Rectangle<float>& area,
                       float startAngle,
                       float endAngle,
                       float strokeWidth) {
  juce::Path p;
  p.addArc(area.getX(), area.getY(), area.getWidth(), area.getHeight(),
           startAngle, endAngle, true);
  juce::PathStrokeType pst(strokeWidth, juce::PathStrokeType::mitered,
                           juce::PathStrokeType::rounded);
  g.strokePath(p, pst);
}

void DepthSliderLookAndFeel::drawRotarySlider(juce::Graphics& g,
                                              int x,
                                              int y,
                                              int width,
                                              int height,
                                              float sliderPosProportional,
                                              float rotaryStartAngle,
                                              float rotaryEndAngle,
                                              juce::Slider&) {
  //  step 1: Draw the main arc
  juce::Rectangle<int> iArea(x, y, width, height);
  auto fArea = iArea.toFloat();
  g.setColour(juce::Colours::aliceblue);
  auto sliderArea = fArea.reduced(5.0f);
  _strokeArc(g, sliderArea, rotaryStartAngle, rotaryEndAngle, 2.0f);
  auto toAngle = flerp(rotaryStartAngle, rotaryEndAngle, sliderPosProportional);
  // step 2: Draw the thumb
  auto radius = (fArea.getWidth() / 2.0f) * 0.95f;
  juce::Point<float> thumbPoint(
      fArea.getCentreX() +
          radius * std::cos(toAngle - juce::MathConstants<float>::halfPi),
      fArea.getCentreY() +
          radius * std::sin(toAngle - juce::MathConstants<float>::halfPi));
  const float thumbWidth = 9.0f;
  g.setColour(juce::Colours::darkseagreen);
  g.fillEllipse(
      juce::Rectangle<float>(thumbWidth, thumbWidth).withCentre(thumbPoint));
}

ModSelectButton::ModSelectButton(int* sel, int idx, int source)
    : WedgeButton(String(source) + "select"),
      selectedSrc(sel),
      index(idx),
      srcID(source) {}

ModSelectButton::~ModSelectButton() {
  auto* parent = getParentComponent();
  if (parent != nullptr) {
    parent->removeChildComponent(this);
  }
}
void ModSelectButton::paintButton(juce::Graphics& g, bool over, bool) {
  auto pBounds = getParentComponent()->getLocalBounds().toFloat();
  auto fX = (float)getX();
  auto fY = (float)getY();
  auto path = getWedgePath(pBounds.getCentreX(), pBounds.getCentreY(), a1, a2,
                           r1, r2, fX, fY);
  // fill
  auto col = Color::getModSourceColor((ModSourceE)srcID);
  auto fillColor = (over || *selectedSrc == srcID) ? col : col.darker(0.3f);
  g.setColour(fillColor);
  g.fillPath(path);
  // stroke
  juce::PathStrokeType pst(1.5f);
  g.setColour(juce::Colours::black);
  g.strokePath(path, pst);
}

void ModCloseButton::paintButton(juce::Graphics& g, bool over, bool) {
  auto pBounds = getParentComponent()->getLocalBounds().toFloat();
  auto fX = (float)getX();
  auto fY = (float)getY();
  auto path = getWedgePath(pBounds.getCentreX(), pBounds.getCentreY(), a1, a2,
                           r1, r2, fX, fY);
  // fill
  auto fillColor = over ? Color::closeRedBright : Color::closeRedDark;
  g.setColour(fillColor);
  g.fillPath(path);
  // stroke
  g.setColour(juce::Colours::black);
  juce::PathStrokeType pst(1.8f);
  g.strokePath(path, pst);
}

static juce::Slider::RotaryParameters _getDepthSliderParams() {
  juce::Slider::RotaryParameters params;
  params.startAngleRadians = juce::MathConstants<float>::pi * 1.35f;
  params.endAngleRadians = juce::MathConstants<float>::pi * 2.65f;
  params.stopAtEnd = true;
  return params;
}

DepthSlider::DepthSlider(int src)
    : juce::Slider(juce::Slider::Rotary, juce::Slider::NoTextBox),
      sourceID(src) {
  setLookAndFeel(&lnf);
  setRange(-1.0f, 1.0f, 0.0001f);
  setValue(0.0f);
  setRotaryParameters(_getDepthSliderParams());
}

DepthSlider::~DepthSlider() {
  auto* parent = getParentComponent();
  if (parent != nullptr)
    parent->removeChildComponent(this);
  setLookAndFeel(nullptr);
}

//===================================================

void DepthSliderStack::_setSliderForSrc(int src, float value) {
  for (auto* s : sliders) {
    if (s->sourceID == src && !fequal((float)s->getValue(), value, 0.001f)) {
      s->setValue((double)value, juce::dontSendNotification);
    }
  }
}

bool DepthSliderStack::_trySimpleConform() {
  auto modulations = state->getModulationTree();
  // count down the sources as we find them
  // so we know if we have extra components no longer
  // attached to the ValueTree
  int numSources = selectButtons.size();
  for (auto it = modulations.begin(); it != modulations.end(); ++it) {
    auto mod = *it;
    const int modDest = mod[ID::modDestID];
    if (modDest == destID) {
      const int srcID = mod[ID::modSourceID];
      if (!hasComponentsForSrc(srcID)) {
        return false;
      }
      --numSources;
      const float _depth = mod[ID::modDepth];
      _setSliderForSrc(srcID, _depth);
    }
  }
  return (numSources == 0);
}

bool DepthSliderStack::hasComponentsForSrc(int src) {
  for (auto* selBtn : selectButtons) {
    if (selBtn->srcID == src) {
      DLog::log("DSS already has modulation from source " + String(src));
      return true;
    }
  }
  return false;
}

void DepthSliderStack::_reindexButtons() {
  int numSliders = 0;
  for (auto* b : selectButtons) {
    if (b->getIndex() > numSliders) {
      b->setIndex(numSliders);
      ++numSliders;
    }
  }
}

// This does the handling of making things visible/not
// Passing in a vaue of -1 indicated that there are no
void DepthSliderStack::_selectSource(int src) {
  selectedSrc = src;
  _setSelectedDepthSlider(src);
  resized();
  repaint();
}

void DepthSliderStack::_setSelectedDepthSlider(int srcID) {
  // do nothing if this slider has already been selected
  if (selectedSlider != nullptr && selectedSlider->sourceID == srcID)
    return;
  for (auto* s : sliders) {
    if (s->sourceID == srcID) {
      selectedSlider = s;
      s->setVisible(true);
      s->setEnabled(true);
      s->toFront(false);
    } else {
      s->setVisible(false);
      s->setEnabled(false);
    }
  }
}

DepthSlider* DepthSliderStack::_sliderForSrc(int src) {
  for (auto* s : sliders) {
    if (s->sourceID == src)
      return s;
  }
  jassert(false);
  return nullptr;
}

ModSelectButton* DepthSliderStack::_btnForSource(int src) {
  for (auto* b : selectButtons) {
    if (b->srcID == src) {
      return b;
    }
  }
  jassert(false);
  return nullptr;
}

//-------------------------------------------------------
void DepthSliderStack::addModulation(int src) {
  // 1. if this is the first modulation we need to turn the close button on
  if (!closeButton.isVisible()) {
    closeButton.setVisible(true);
    closeButton.setEnabled(true);
  }
  // 2. create a new select button and give it a callback lambda
  int buttonIdx = selectButtons.size();
  auto* btn =
      selectButtons.add(new ModSelectButton(&selectedSrc, buttonIdx, src));
  btn->onClick = [this, src]() { this->_selectSource(src); };
  addAndMakeVisible(btn);
  // 3. create a new depth slider and attach the listener
  auto* slider = sliders.add(new DepthSlider(src));
  slider->addListener(this);
  addAndMakeVisible(slider);
  // 4. update the state
  state->setModulation(src, destID, 0.0f);
  // 5. make sure our new mod source is selected
  _selectSource(src);
}

void DepthSliderStack::removeModulation(int src) {
  if (!hasComponentsForSrc(src)) {
    DLog::log("Dest " + String(destID) + " not modulated by src " +
              String(src));
    return;
  }
  // 1. handle the case where we're deleting the selected
  // source
  if (src == selectedSrc) {
    // if we're about to delete the last source,
    // switch the pointers to indicate that
    if (sliders.size() < 2) {
      selectedSrc = -1;
      selectedSlider = nullptr;
      // also disable the close button here
      closeButton.setVisible(false);
      closeButton.setEnabled(false);
    } else {
      selectedSrc = sliders[sliders.size() - 2]->sourceID;
    }
  }
  // 2. remove the slider
  sliders.removeObject(_sliderForSrc(src));
  // 3. remove the select button (and reindex)
  selectButtons.removeObject(_btnForSource(src));
  _reindexButtons();
  // 4. update the state
  state->removeModulation(src, destID);
  // 5. update the GUI as appropriate
  if (selectedSrc != -1) {
    _selectSource(selectedSrc);
  } else {
    resized();
  }
}

void DepthSliderStack::reinitFromState() {
  // if we haven't added or removed any modulations
  // we should be able to get away with what I'm calling a
  // "simple conform", otherwise we'll reset the ownedArrays
  // based on the shared state
  if (!_trySimpleConform()) {
    // empty the arrays but don't call any listener/GUI stuff yet
    selectedSlider = nullptr;
    sliders.clear();
    selectButtons.clear();
    // 1. traverse the modulations tree to find anything we need to add
    auto modulations = state->getModulationTree();
    for (auto it = modulations.begin(); it != modulations.end(); ++it) {
      auto mod = *it;
      int _dest = mod[ID::modDestID];
      if (_dest == destID) {
        const int _src = mod[ID::modSourceID];
        const float _depth = mod[ID::modDepth];
        // add the slider
        auto* slider = sliders.add(new DepthSlider(_src));
        addAndMakeVisible(slider);
        slider->setVisible(false);
        slider->setEnabled(false);
        _setSliderForSrc(_src, _depth);
        slider->addListener(this);
        // add the select button
        auto* btn = selectButtons.add(
            new ModSelectButton(&selectedSrc, selectButtons.size(), _src));
        addAndMakeVisible(btn);
        btn->onClick = [this, _src]() { _selectSource(_src); };
      }
    }
    // now make the appropriate GUI calls
    if (sliders.size() > 0) {
      const int src = sliders.getLast()->sourceID;
      _selectSource(src);
    } else {
      selectedSrc = -1;
      repaint();
    }
  }
}

//-------------------------------------------------------

DepthSliderStack::DepthSliderStack(ElectrumState* s, int dest)
    : state(s), destID(dest) {
  addAndMakeVisible(&closeButton);
  // set up the lambda for the close button
  closeButton.onClick = [this]() {
    if (selectedSrc != -1) {
      removeModulation(selectedSrc);
    }
  };
  closeButton.setVisible(false);
  closeButton.setEnabled(false);
}

void DepthSliderStack::sliderValueChanged(juce::Slider* s) {
  for (auto* slider : sliders) {
    if (slider == s) {
      const float depth = (float)s->getValue();
      state->setModulation(slider->sourceID, destID, depth);
    }
  }
}

void DepthSliderStack::resized() {
  // if we have no sliders, we just need to make sure the close button is hidden
  if (selectedSrc == -1) {
    closeButton.setVisible(false);
    return;
  }
  closeButton.setVisible(true);
  // size all the sliders
  auto lBounds = getLocalBounds().toFloat();
  auto r2 = lBounds.getWidth() / 2.0f;
  auto r1 = r2 - 10.0f;
  float baseAngle = juce::MathConstants<float>::pi * 0.675f;
  float minButtonAngle = juce::MathConstants<float>::pi / 8.0f;
  float buttonAngle =
      (juce::MathConstants<float>::pi * 0.65f) / (float)(sliders.size() + 1);
  // place the close button
  closeButton.setStartAngle(baseAngle);
  closeButton.setEndAngle(baseAngle + minButtonAngle);
  closeButton.setInnerRadius(r1);
  closeButton.setOuterRadius(r2);
  auto closeBounds =
      closeButton
          .getWedgePathForCenter(lBounds.getCentreX(), lBounds.getCentreY())
          .getBounds();
  closeButton.setBounds(closeBounds.toNearestInt());
  closeButton.toFront(false);
  // place the slider and select buttons
  baseAngle += minButtonAngle;
  for (int i = 0; i < sliders.size(); i++) {
    sliders[i]->setBounds(lBounds.toNearestInt());
    if (sliders[i]->sourceID == selectedSrc) {
      sliders[i]->setVisible(true);
      sliders[i]->setEnabled(true);
      // sliders[i]->toFront(false);
      selectButtons[i]->setEnabled(false);
    } else {
      sliders[i]->setVisible(false);
      selectButtons[i]->toFront(false);
      selectButtons[i]->setEnabled(true);
    }
    // place the select button
    auto angle = baseAngle + (buttonAngle * (float)i);
    selectButtons[i]->setStartAngle(angle);
    selectButtons[i]->setEndAngle(angle + buttonAngle);
    selectButtons[i]->setInnerRadius(r1);
    selectButtons[i]->setOuterRadius(r2);
    auto bBounds =
        selectButtons[i]
            ->getWedgePathForCenter(lBounds.getCentreX(), lBounds.getCentreY())
            .getBounds();
    selectButtons[i]->setBounds(bBounds.toNearestInt());
  }
}

//======================================================================================
//===================================================

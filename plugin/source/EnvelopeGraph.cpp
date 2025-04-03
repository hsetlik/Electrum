#include "Electrum/GUI/ModulatorPanel/EnvelopeGraph.h"
#include "Electrum/GUI/GUITypedefs.h"
#include "Electrum/GUI/LookAndFeel/Color.h"
// geometry helpers----------------------------------

static float getMaxAttackLength(frect_t& bounds) {
  return FATTACK * bounds.getWidth();
}

static float getMaxHoldLength(frect_t& bounds) {
  return FHOLD * bounds.getWidth();
}
static float getMaxDecayLength(frect_t& bounds) {
  return FDECAY * bounds.getWidth();
}
static float getMaxReleaseLength(frect_t& bounds) {
  return FRELEASE * bounds.getWidth();
}
static float sustainLevelY(frect_t& bounds, float val) {
  float min = bounds.getY() + 5.0f;
  float max = bounds.getBottom() - 5.0f;
  return juce::jmap(1.0f - val, 0.0f, 1.0f, min, max);
}
static float sustainFromY(frect_t& bounds, float y) {
  float min = bounds.getY() + 5.0f;
  float max = bounds.getBottom() - 5.0f;
  return 1.0f - juce::jmap(y, min, max, 0.0f, 1.0f);
}

//===================================================
static float onEasingCurve(float y0, float y1, float y2, float x) {
  float yM = juce::jmap(y1, y0, y2, 0.0f,
                        1.0f);  // normalize y1 between 0 and 1 to use the
                                // exponential parent function
  // this is a basic exponential function where we know that:
  //     f(x) = x^t
  //     f(0.5) = yM
  //     0.5^t = yM
  //     log<0.5>(yM) = t
  float t = std::log(yM) / std::log(0.5f);
  float val = std::pow(x, t);
  return flerp(y0, y2, val);  // de-normalize from 0-1 range back to full range
                              // (undo the jmap in other words)
}

static void drawHandle(juce::Graphics& g,
                       fpoint_t center,
                       float radius,
                       bool isSelected) {
  frect_t bounds(radius * 2.0f, radius * 2.0f);
  bounds = bounds.withCentre(center);
  g.setColour(Color::literalOrangePale);
  if (isSelected) {
    g.fillEllipse(bounds);
  } else {
    g.drawEllipse(bounds, HANDLE_STROKE);
  }
}

void EnvelopeGraph::drawEnvelopeGraph(frect_t& bounds, juce::Graphics& g) {
  static const int curvePoints = 60;
  if (bounds.getHeight() < 1.0f || bounds.getWidth() < 1.0f) {
    return;
  }
  // draw the path
  juce::Path p;
  p.startNewSubPath(bounds.getX(), bounds.getBottom());
  // draw the attack curve
  juce::Path p2;
  p2.startNewSubPath(bounds.getX(), bounds.getBottom());
  const float yMax = bounds.getHeight() - 5.0f;
  for (int i = 0; i < curvePoints; i++) {
    // DLog::log("Drawing attack curve point #" + String(i));
    float t = ((float)i / (float)curvePoints);
    float fX = t * attackEnd.getX();
    float fY = bounds.getBottom() - 5.0f -
               onEasingCurve(0.0f, yMax - attackCurve.getY(), yMax, t);
    fY = std::max({fY, 5.0f});
    p.lineTo(fX, fY);
  }

  p.lineTo(attackEnd.getX(), attackEnd.getY());

  p.lineTo(holdEnd.getX(), holdEnd.getY());

  for (int i = 0; i < curvePoints; i++) {
    float t = ((float)i / (float)curvePoints);
    float fX = flerp(holdEnd.getX(), decayEnd.getX(), t);
    float fY = decayEnd.getY() -
               onEasingCurve(0.0f, decayEnd.getY() - decayCurve.getY(),
                             decayEnd.getY(), 1.0f - t);
    if (std::isnan(fY)) {
      // we'll fix this by brute force by just getting the sustain value from
      // the state
      const float bottom = state
                               ->getRawParameterValue(
                                   ID::sustainLevel.toString() + String(envID))
                               ->load() *
                           bounds.getHeight();
      fY = bottom -
           onEasingCurve(0.0f, bottom - decayCurve.getY(), bottom, 1.0f - t);
    }
    fY = std::max(fY, holdEnd.getY());
    if (std::isnan(fX) || std::isnan(fY)) {
      DLog::log("NAN coords on decay curve at point: " + String(i));
    } else
      p.lineTo(fX, fY);
  }
  p.lineTo(decayEnd.getX(), decayEnd.getY());
  p.lineTo(sustainEnd.getX(), sustainEnd.getY());
  // DLog::log("Drawing line to bottom corner");
  for (int i = 0; i < curvePoints; i++) {
    float t = ((float)i / (float)curvePoints);
    float fX = flerp(sustainEnd.getX(), bounds.getRight(), t);
    float bottom = bounds.getBottom();
    float height = bottom - sustainEnd.getY();
    float yCurve = bottom - releaseCurve.getY();
    float fY = bottom - onEasingCurve(0.0f, yCurve, height, 1.0f - t);
    p.lineTo(fX, fY);
  }
  p.lineTo(bounds.getRight(), bounds.getBottom());
  g.setColour(Color::literalOrangePale);
  juce::PathStrokeType pst(1.2f);
  g.strokePath(p, pst);

  // draw the handles
  drawHandle(g, attackEnd.getPos(), 3.0f, selectedPoint != &attackEnd);
  drawHandle(g, attackCurve.getPos(), 3.0f, selectedPoint != &attackCurve);
  drawHandle(g, holdEnd.getPos(), 3.0f, selectedPoint != &holdEnd);
  drawHandle(g, decayCurve.getPos(), 3.0f, selectedPoint != &decayCurve);
  drawHandle(g, decayEnd.getPos(), 3.0f, selectedPoint != &decayEnd);
  drawHandle(g, sustainEnd.getPos(), 3.0f, selectedPoint != &sustainEnd);
  drawHandle(g, releaseCurve.getPos(), 3.0f, selectedPoint != &releaseCurve);
}

//===================================================
EnvelopeGraph::EnvelopeGraph(ElectrumState* s, int id)
    : state(s),
      envID(id),
      attackEnd(this),
      attackCurve(this),
      holdEnd(this),
      decayCurve(this),
      decayEnd(this),
      sustainEnd(this),
      releaseCurve(this) {
  // 1. sync with state such that the DragPoints are in legal spots
  syncWithState();
  // 2. grip the identifiers and set up the attachments
  String iStr(id);
  String atkMsID = ID::attackMs.toString() + iStr;

  attackMsAttach.reset(new DragPointAttach(
      state, atkMsID, &attackEnd,
      [this, atkMsID](fpoint_t pt) {
        return getParamFromPos(atkMsID, &attackEnd, pt);
      },
      [this, atkMsID](float val) {
        return getPosFromParam(atkMsID, &attackEnd, val);
      }));

  String atkCurveID = ID::attackCurve.toString() + iStr;
  attackCurveAttach.reset(new DragPointAttach(
      state, atkCurveID, &attackCurve,
      [this, atkCurveID](fpoint_t pt) {
        return getParamFromPos(atkCurveID, &attackCurve, pt);
      },
      [this, atkCurveID](float val) {
        return getPosFromParam(atkCurveID, &attackCurve, val);
      }));

  String holdId = ID::holdMs.toString() + iStr;
  holdMsAttach.reset(new DragPointAttach(
      state, holdId, &holdEnd,
      [this, holdId](juce::Point<float> pt) {
        return getParamFromPos(holdId, &holdEnd, pt);
      },
      [this, holdId](float val) {
        return getPosFromParam(holdId, &holdEnd, val);
      }));

  String decayMsID = ID::decayMs.toString() + iStr;
  decayMsAttach.reset(new DragPointAttach(
      state, decayMsID, &decayEnd,
      [this, decayMsID](juce::Point<float> pt) {
        return getParamFromPos(decayMsID, &decayEnd, pt);
      },
      [this, decayMsID](float val) {
        return getPosFromParam(decayMsID, &decayEnd, val);
      }));

  String decayCurveID = ID::decayCurve.toString() + iStr;
  decayCurveAttach.reset(new DragPointAttach(
      state, decayCurveID, &decayCurve,
      [this, decayCurveID](juce::Point<float> pt) {
        return getParamFromPos(decayCurveID, &decayCurve, pt);
      },
      [this, decayCurveID](float val) {
        return getPosFromParam(decayCurveID, &decayCurve, val);
      }));

  String sustainId = ID::sustainLevel.toString() + iStr;
  sustainAttach1.reset(new DragPointAttach(
      state, sustainId, &decayEnd,
      [this, sustainId](juce::Point<float> pt) {
        return getParamFromPos(sustainId, &decayEnd, pt);
      },
      [this, sustainId](float val) {
        return getPosFromParam(sustainId, &decayEnd, val);
      }));

  sustainAttach2.reset(new DragPointAttach(
      state, sustainId, &sustainEnd,
      [this, sustainId](juce::Point<float> pt) {
        return getParamFromPos(sustainId, &sustainEnd, pt);
      },
      [this, sustainId](float val) {
        return getPosFromParam(sustainId, &sustainEnd, val);
      }));

  String releaseId = ID::releaseMs.toString() + iStr;
  releaseMsAttach.reset(new DragPointAttach(
      state, releaseId, &sustainEnd,
      [this, releaseId](juce::Point<float> pt) {
        return getParamFromPos(releaseId, &sustainEnd, pt);
      },
      [this, releaseId](float val) {
        return getPosFromParam(releaseId, &sustainEnd, val);
      }));

  String releaseCurveID = ID::releaseCurve.toString() + iStr;
  releaseCurveAttach.reset(new DragPointAttach(
      state, releaseCurveID, &releaseCurve,
      [this, releaseCurveID](juce::Point<float> pt) {
        return getParamFromPos(releaseCurveID, &releaseCurve, pt);
      },
      [this, releaseCurveID](float val) {
        return getPosFromParam(releaseCurveID, &releaseCurve, val);
      }));
  triggerAsyncUpdate();
}

void EnvelopeGraph::paint(juce::Graphics& g) {
  auto fBounds = getLocalBounds().toFloat();
  drawEnvelopeGraph(fBounds, g);
}

void EnvelopeGraph::handleAsyncUpdate() {
  syncWithState();
  repaint();
}

// Mouse callbacks=======================

void EnvelopeGraph::mouseDown(const juce::MouseEvent& e) {
  for (auto* p : points) {
    if (p->isWithin(e, 3.0f)) {
      selectedPoint = p;
      return;
    }
  }
  selectedPoint = nullptr;
  isMoving = false;
}

void EnvelopeGraph::mouseDrag(const juce::MouseEvent& e) {
  if (selectedPoint != nullptr) {
    if (!isMoving) {
      isMoving = true;
      selectedPoint->startMove();
    }
    auto destPos = constrainPositionFor(selectedPoint, e.position);
    selectedPoint->movePoint(destPos.x, destPos.y);
    triggerAsyncUpdate();
  } else
    isMoving = false;
}

void EnvelopeGraph::mouseUp(const juce::MouseEvent& e) {
  if (selectedPoint != nullptr) {
    selectedPoint->endMove();
    triggerAsyncUpdate();
  }
  selectedPoint = nullptr;
}

// Callback builders------------------

fpoint_t EnvelopeGraph::getPosFromParam(const String& paramID,
                                        DragPoint* point,
                                        float value) {
  auto fBounds = getLocalBounds().toFloat();
  const float yTop = fBounds.getY() + 5.0f;

  if (paramID.contains(ID::attackMs.toString())) {
    auto range = state->getParameterRange(paramID);
    if (!range.getRange().contains(value)) {
      DLog::log("Out of range attack value: " + String(value));
    }
    auto xAtk = range.convertTo0to1(value) * getMaxAttackLength(fBounds);
    return {xAtk, yTop};
  } else if (paramID.contains(ID::attackCurve.toString())) {
    float x0 = 0.0f;
    float y0 = yTop;
    float x1 = attackEnd.getX();
    float y1 = fBounds.getBottom() - 5.0f;
    float xPos = flerp(x0, x1, 0.5f);
    float yPos = flerp(y1, y0, value);
    return {xPos, yPos};
  } else if (paramID.contains(ID::holdMs.toString())) {
    auto range = state->getParameterRange(paramID);
    // we need to offset this by the attack x value
    auto xHold = (range.convertTo0to1(value) * getMaxHoldLength(fBounds)) +
                 (attackEnd.getX() + HOLD_BUF);
    return {xHold, yTop};
  } else if (paramID.contains(ID::decayCurve.toString())) {
    float x0 = holdEnd.getX();
    float y0 = yTop;
    float x1 = decayEnd.getX();
    float y1 = decayEnd.getY();
    float yPos = flerp(y1, y0, value);
    if (std::isnan(yPos) || std::isnan(x0) || std::isnan(x1)) {
      DLog::log("DECAY CURVE HAS INVALID LOCATION!");
    }
    return {flerp(x0, x1, 0.5f), yPos};
  } else if (paramID.contains(ID::decayMs.toString())) {
    // since sustain hasn't been passed to this function, the y positions for
    // the last two points just keep their existing values
    auto range = state->getParameterRange(paramID);
    if (!range.getRange().contains(value)) {
      DLog::log("Out of range decay value: " + String(value));
    }
    auto xDecay = (range.convertTo0to1(value) * getMaxDecayLength(fBounds)) +
                  holdEnd.getX();
    return {xDecay, point->getY()};
  } else if (paramID.contains(ID::sustainLevel.toString())) {
    // since this could be one of two points, we keep the point's previous X
    // value
    auto susY = sustainLevelY(fBounds, value);
    return {point->getX(), susY};
  } else if (paramID.contains(ID::releaseMs.toString())) {
    auto range = state->getParameterRange(paramID);
    if (!range.getRange().contains(value)) {
      // DLog::log("Out of range release value: " + String(value));
      value = range.snapToLegalValue(value);
    }
    auto xRelease = fBounds.getRight() -
                    (range.convertTo0to1(value) * getMaxReleaseLength(fBounds));
    return {xRelease, point->getY()};
  } else if (paramID.contains(ID::releaseCurve.toString())) {
    // DLog::log("Getting position from release curve value: " + String(value));
    float xPos = flerp(sustainEnd.getX(), fBounds.getRight(), 0.5f);
    float yPos = flerp(sustainEnd.getY(), fBounds.getBottom(), 1.0f - value);
    return {xPos, yPos};
  }
  DLog::log("Warning: requested position for invalid param ID: \"" + paramID +
            "\"");
  return {0.0f, 0.0f};
}

float EnvelopeGraph::getParamFromPos(const String& paramID,
                                     DragPoint* point,
                                     fpoint_t pos) {
  auto range = state->getParameterRange(paramID);
  auto fBounds = getLocalBounds().toFloat();
  const float yTop = fBounds.getY() + 5.0f;
  if (point == &attackEnd) {
    auto fAttack = (pos.x / getMaxAttackLength(fBounds));
    if (fAttack < 0.0f || fAttack > 1.0f) {
      DLog::log("FAttack is out of range in EnvelopeGraph.cpp!");
    }
    return range.convertFrom0to1(fAttack);

  } else if (point == &attackCurve) {
    float y0 = yTop;
    float y1 = fBounds.getBottom() - 5.0f;
    return (pos.y - y1) / (y0 - y1);
  } else if (point == &holdEnd) {
    auto fHold =
        (pos.x - (attackEnd.getX() + HOLD_BUF)) / getMaxHoldLength(fBounds);
    if (fHold < 0.0f || fHold > 1.0f) {
      DLog::log("fHold is out of range in EnvelopeGraph.cpp!");
    }
    return range.convertFrom0to1(fHold);
  } else if (point == &decayCurve) {
    float y1 = decayEnd.getY();
    float output;
    if (!fequal(y1, yTop)) {
      output = (pos.y - y1) / (yTop - y1);
    } else {
      output = state->getRawParameterValue(paramID)->load();
    }
    return output;
  } else if (point == &decayEnd)  // careful: what looks like two cases is
                                  // actually four cases because the last two
                                  // points can control 1 of 2 parameters
  {
    if (paramID.contains(ID::sustainLevel.toString()))  // sustain
    {
      return sustainFromY(fBounds, pos.y);
    } else  // decay time
    {
      auto fDecay = (pos.x - holdEnd.getX()) / getMaxDecayLength(fBounds);
      if (fDecay < 0.0f || fDecay > 1.0f) {
        DLog::log("fDecay is out of range in EnvelopeGraph.cpp!");
      }
      return range.convertFrom0to1(fDecay);
    }
  } else if (point == &sustainEnd) {
    if (paramID.contains(ID::sustainLevel.toString()))  // sustain
    {
      return sustainFromY(fBounds, pos.y);
    } else  // release time
    {
      auto fRelease =
          (fBounds.getRight() - pos.x) / getMaxReleaseLength(fBounds);
      if (fRelease < 0.0f || fRelease > 1.0f) {
        DLog::log("fRelease is out of range in EnvelopeGraph.cpp!");
      }
      return range.convertFrom0to1(fRelease);
    }
  } else if (point == &releaseCurve) {
    // DLog::log("Calculating release value from Y position: " + String(pos.y));
    float yMax = fBounds.getBottom();
    float yMin = sustainEnd.getY();
    // DLog::log("Y range is between " + String(yMin) + " and " + String(yMax));
    return 1.0f - ((pos.y - yMin) / (yMax - yMin));
  }
  DLog::log("Warning- no point matching " +
            String(reinterpret_cast<const char*>(point)) + " was found!");
  return 0.0f;
}

// Position/Constrainer
// stuff=======================================================
frect_t EnvelopeGraph::getLimitsFor(DragPoint* pt) {
  auto fBounds = getLocalBounds().toFloat();
  auto yTop = fBounds.getY() + 5.0f;
  auto yBottom = fBounds.getBottom() - 5.0f;
  float yHeight = yBottom - yTop;
  if (pt == &attackEnd) {
    return {0.0f, yTop, getMaxAttackLength(fBounds), 0.0f};
  } else if (pt == &attackCurve) {
    auto cX = attackEnd.getX() / 2.0f;
    return {cX, yTop, 0.0f, yHeight};
  } else if (pt == &holdEnd) {
    auto xMin = attackEnd.getX() + HOLD_BUF;
    return {xMin, yTop, getMaxHoldLength(fBounds), 0.0f};
  } else if (pt == &decayCurve) {
    auto xMin = flerp(holdEnd.getX(), decayEnd.getX(), 0.5f);
    auto range = decayEnd.getY() - yTop;
    return {xMin, yTop, 0.0f, range};
  } else if (pt == &decayEnd) {
    auto xMin = holdEnd.getX();
    return {xMin, yTop, getMaxDecayLength(fBounds), yHeight};
  } else if (pt == &sustainEnd) {
    auto width = getMaxReleaseLength(fBounds);
    return {fBounds.getRight() - width, yTop, width, yHeight};
  } else  // release curve
  {
    float xMin = flerp(sustainEnd.getX(), fBounds.getRight(), 0.5f);
    float range = fBounds.getHeight() - sustainEnd.getY();
    return {xMin, sustainEnd.getY(), 0.0f, range};
  }
}

fpoint_t EnvelopeGraph::constrainPositionFor(DragPoint* point, fpoint_t pos) {
  auto bounds = getLimitsFor(point);
  return bounds.getConstrainedPoint(pos);
}

//-----------------------------------------

void EnvelopeGraph::syncWithState() {
  String iStr(envID);
  auto attackMsID = ID::attackMs.toString() + iStr;
  auto attackCurveID = ID::attackCurve.toString() + iStr;
  auto holdID = ID::holdMs.toString() + iStr;
  auto decayCurveID = ID::decayCurve.toString() + iStr;
  auto decayID = ID::decayMs.toString() + iStr;
  auto sustainID = ID::sustainLevel.toString() + iStr;
  auto releaseID = ID::releaseMs.toString() + iStr;
  auto releaseCurveID = ID::releaseCurve.toString() + iStr;

  const float attackMs = state->getFloatParamValue(attackMsID);
  const float attackCurveValue = state->getFloatParamValue(attackCurveID);
  const float holdMs = state->getFloatParamValue(holdID);
  const float decayCurveValue = state->getFloatParamValue(decayCurveID);
  const float decayMs = state->getFloatParamValue(decayID);
  const float sustainLevel = state->getFloatParamValue(sustainID);
  const float releaseMs = state->getFloatParamValue(releaseID);
  const float releaseCurveValue = state->getFloatParamValue(releaseCurveID);
  // check each parameter, update if it's changed
  auto newAtkPos = getPosFromParam(attackMsID, &attackEnd, attackMs);
  if (newAtkPos != attackEnd.getPos()) {
    // DLog::log("Updating attack position");
    attackEnd.moveTo(newAtkPos);
  }

  auto aCurvePos =
      getPosFromParam(attackCurveID, &attackCurve, attackCurveValue);
  if (aCurvePos != attackCurve.getPos()) {
    attackCurve.moveTo(aCurvePos);
  }

  auto holdPos = getPosFromParam(holdID, &holdEnd, holdMs);
  if (holdPos != holdEnd.getPos()) {
    // DLog::log("Updating hold position");
    holdEnd.moveTo(holdPos);
  }
  auto sustainPos = getPosFromParam(sustainID, &decayEnd, sustainLevel);
  auto decayPos = getPosFromParam(decayID, &decayEnd, decayMs);
  decayPos.y = sustainPos.y;
  if (decayPos != decayEnd.getPos())  // decay time has changed
  {
    // DLog::log("Updating decay position");
    decayEnd.moveTo(decayPos);
  }

  auto dCurvePos = getPosFromParam(decayCurveID, &decayCurve, decayCurveValue);
  if (dCurvePos != decayCurve.getPos()) {
    // DLog::log("Moving decay curve to: " + dCurvePos.toString());
    decayCurve.moveTo(dCurvePos);
  }
  auto releasePos = getPosFromParam(releaseID, &sustainEnd, releaseMs);
  releasePos.y = sustainPos.y;
  if (releasePos != sustainEnd.getPos()) {
    // DLog::log("Updating release position");
    sustainEnd.moveTo(releasePos);
  }
  auto releaseCurvePos =
      getPosFromParam(releaseCurveID, &releaseCurve, releaseCurveValue);
  if (releaseCurvePos != releaseCurve.getPos()) {
    // DLog::log("Updating release curve position: " +
    // String(releaseCurvePos.toString()));
    releaseCurve.moveTo(releaseCurvePos);
  }
}

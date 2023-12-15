#include "EnvelopeGraph.h"
EnvelopeGraphCore::EnvelopeGraphCore(EVT *tree, int idx)
    : state(tree), index(idx), attackEnd(this), attackCurve(this),
      holdEnd(this), decayCurve(this), decayEnd(this), sustainEnd(this),
      releaseCurve(this), selectedPoint(nullptr), isMoving(false)
{
  // sync with state up here so that the DragPoints are at the correct positions
  // when the attachments are instantiated
  syncWithState();
  String iStr(index);
  String atkMsID = IDs::attackMs.toString() + iStr;

  attackMsAttach.reset(new DragPointAttachment(
      state, atkMsID, &attackEnd,
      [this, atkMsID](Point<float> pt) {
        return getParamFromPos(atkMsID, &attackEnd, pt);
      },
      [this, atkMsID](float val) {
        return getPosFromParam(atkMsID, &attackEnd, val);
      }));

  String atkCurveID = IDs::attackCurve.toString() + iStr;
  attackCurveAttach.reset(new DragPointAttachment(
      state, atkCurveID, &attackCurve,
      [this, atkCurveID](Point<float> pt) {
        return getParamFromPos(atkCurveID, &attackCurve, pt);
      },
      [this, atkCurveID](float val) {
        return getPosFromParam(atkCurveID, &attackCurve, val);
      }));

  String holdId = IDs::holdMs.toString() + iStr;
  holdMsAttach.reset(new DragPointAttachment(
      state, holdId, &holdEnd,
      [this, holdId](Point<float> pt) {
        return getParamFromPos(holdId, &holdEnd, pt);
      },
      [this, holdId](float val) {
        return getPosFromParam(holdId, &holdEnd, val);
      }));

  String decayMsID = IDs::decayMs.toString() + iStr;
  decayMsAttach.reset(new DragPointAttachment(
      state, decayMsID, &decayEnd,
      [this, decayMsID](Point<float> pt) {
        return getParamFromPos(decayMsID, &decayEnd, pt);
      },
      [this, decayMsID](float val) {
        return getPosFromParam(decayMsID, &decayEnd, val);
      }));

  String decayCurveID = IDs::decayCurve.toString() + iStr;
  decayCurveAttach.reset(new DragPointAttachment(
      state, decayCurveID, &decayCurve,
      [this, decayCurveID](Point<float> pt) {
        return getParamFromPos(decayCurveID, &decayCurve, pt);
      },
      [this, decayCurveID](float val) {
        return getPosFromParam(decayCurveID, &decayCurve, val);
      }));

  String sustainId = IDs::sustainLevel.toString() + iStr;
  sustainAttach1.reset(new DragPointAttachment(
      state, sustainId, &decayEnd,
      [this, sustainId](Point<float> pt) {
        return getParamFromPos(sustainId, &decayEnd, pt);
      },
      [this, sustainId](float val) {
        return getPosFromParam(sustainId, &decayEnd, val);
      }));

  sustainAttach2.reset(new DragPointAttachment(
      state, sustainId, &sustainEnd,
      [this, sustainId](Point<float> pt) {
        return getParamFromPos(sustainId, &sustainEnd, pt);
      },
      [this, sustainId](float val) {
        return getPosFromParam(sustainId, &sustainEnd, val);
      }));

  String releaseId = IDs::releaseMs.toString() + iStr;
  releaseMsAttach.reset(new DragPointAttachment(
      state, releaseId, &sustainEnd,
      [this, releaseId](Point<float> pt) {
        return getParamFromPos(releaseId, &sustainEnd, pt);
      },
      [this, releaseId](float val) {
        return getPosFromParam(releaseId, &sustainEnd, val);
      }));

  String releaseCurveID = IDs::releaseCurve.toString() + iStr;
  releaseCurveAttach.reset(new DragPointAttachment(
      state, releaseCurveID, &releaseCurve,
      [this, releaseCurveID](Point<float> pt) {
        return getParamFromPos(releaseCurveID, &releaseCurve, pt);
      },
      [this, releaseCurveID](float val) {
        return getPosFromParam(releaseCurveID, &releaseCurve, val);
      }));

  triggerAsyncUpdate();
}

void EnvelopeGraphCore::paint(Graphics &g)
{
  auto fBounds = getLocalBounds().toFloat();
  drawEnvelopeGraph(fBounds, g);
}

void EnvelopeGraphCore::handleAsyncUpdate()
{
  syncWithState();
  repaint();
}

//======================================================================================
void EnvelopeGraphCore::drawEnvelopeGraph(Rectangle<float> &bounds, Graphics &g)
{
  const int curvePoints = 60;
  if (bounds.getHeight() < 1.0f || bounds.getWidth() < 1.0f)
  {
    return;
  }
  // draw the path
  Path p;
  p.startNewSubPath(bounds.getX(), bounds.getBottom());
  // draw the attack curve
  Path p2;
  p2.startNewSubPath(bounds.getX(), bounds.getBottom());
  const float yMax = bounds.getHeight() - 5.0f;
  for (int i = 0; i < curvePoints; i++)
  {
    // DLog::log("Drawing attack curve point #" + String(i));
    float t = ((float)i / (float)curvePoints);
    float fX = t * attackEnd.getX();
    float fY = bounds.getBottom() - 5.0f -
               Math::onEasingCurve(0.0f, yMax - attackCurve.getY(), yMax, t);
    fY = std::max({fY, 5.0f});
    p.lineTo(fX, fY);
  }

  p.lineTo(attackEnd.getX(), attackEnd.getY());

  p.lineTo(holdEnd.getX(), holdEnd.getY());

  for (int i = 0; i < curvePoints; i++)
  {
    float t = ((float)i / (float)curvePoints);
    float fX = Math::flerp(holdEnd.getX(), decayEnd.getX(), t);
    float fY = decayEnd.getY() -
               Math::onEasingCurve(0.0f, decayEnd.getY() - decayCurve.getY(),
                                   decayEnd.getY(), 1.0f - t);
    if (std::isnan(fY))
    {
      // we'll fix this by brute force by just getting the sustain value from
      // the state
      const float bottom = state->getFloatParamValue(
                               IDs::sustainLevel.toString() + String(index)) *
                           bounds.getHeight();
      fY = bottom - Math::onEasingCurve(0.0f, bottom - decayCurve.getY(),
                                        bottom, 1.0f - t);
    }
    fY = std::max(fY, holdEnd.getY());
    if (std::isnan(fX) || std::isnan(fY))
    {
      DLog::log("NAN coords on decay curve at point: " + String(i));
    } else
      p.lineTo(fX, fY);
  }
  p.lineTo(decayEnd.getX(), decayEnd.getY());
  p.lineTo(sustainEnd.getX(), sustainEnd.getY());
  // DLog::log("Drawing line to bottom corner");
  for (int i = 0; i < curvePoints; i++)
  {
    float t = ((float)i / (float)curvePoints);
    float fX = Math::flerp(sustainEnd.getX(), bounds.getRight(), t);
    float bottom = bounds.getBottom();
    float height = bottom - sustainEnd.getY();
    float yCurve = bottom - releaseCurve.getY();
    float fY = bottom - Math::onEasingCurve(0.0f, yCurve, height, 1.0f - t);
    p.lineTo(fX, fY);
  }
  p.lineTo(bounds.getRight(), bounds.getBottom());
  g.setColour(Color::brightYellow);
  PathStrokeType pst(1.2f);
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

void EnvelopeGraphCore::mouseDown(const MouseEvent &e)
{
  for (auto *p : points)
  {
    if (p->isWithin(e, 3.0f))
    {
      selectedPoint = p;
      return;
    }
  }
  selectedPoint = nullptr;
  isMoving = false;
}

void EnvelopeGraphCore::mouseDrag(const MouseEvent &e)
{
  if (selectedPoint != nullptr)
  {
    if (!isMoving)
    {
      isMoving = true;
      selectedPoint->startMove();
    }
    auto destPos = constrainPositionFor(selectedPoint, e.position);
    selectedPoint->movePoint(destPos.x, destPos.y);
    triggerAsyncUpdate();
  } else
    isMoving = false;
}

void EnvelopeGraphCore::mouseUp(const MouseEvent &)
{
  if (selectedPoint != nullptr)
  {
    selectedPoint->endMove();
    triggerAsyncUpdate();
  }
  selectedPoint = nullptr;
}

//============================================================

Point<float> EnvelopeGraphCore::getPosFromParam(const String &paramID,
                                                DragPoint *point, float value)
{
  auto fBounds = getLocalBounds().toFloat();
  const float yTop = fBounds.getY() + 5.0f;

  if (paramID.contains(IDs::attackMs.toString()))
  {
    auto range = state->getAPVTS()->getParameterRange(paramID);
    if (!range.getRange().contains(value))
    {
      DLog::log("Out of range attack value: " + String(value));
    }
    auto xAtk = range.convertTo0to1(value) * getMaxAttackLength(fBounds);
    return {xAtk, yTop};
  } else if (paramID.contains(IDs::attackCurve.toString()))
  {
    float x0 = 0.0f;
    float y0 = yTop;
    float x1 = attackEnd.getX();
    float y1 = fBounds.getBottom() - 5.0f;
    float xPos = Math::flerp(x0, x1, 0.5f);
    float yPos = Math::flerp(y1, y0, value);
    return {xPos, yPos};
  } else if (paramID.contains(IDs::holdMs.toString()))
  {
    auto range = state->getAPVTS()->getParameterRange(paramID);
    // we need to offset this by the attack x value
    auto xHold = (range.convertTo0to1(value) * getMaxHoldLength(fBounds)) +
                 (attackEnd.getX() + HOLD_BUF);
    return {xHold, yTop};
  } else if (paramID.contains(IDs::decayCurve.toString()))
  {

    float x0 = holdEnd.getX();
    float y0 = yTop;
    float x1 = decayEnd.getX();
    float y1 = decayEnd.getY();
    float yPos = Math::flerp(y1, y0, value);
    if (std::isnan(yPos) || std::isnan(x0) || std::isnan(x1))
    {
      DLog::log("DECAY CURVE HAS INVALID LOCATION!");
    }
    return {Math::flerp(x0, x1, 0.5f), yPos};
  } else if (paramID.contains(IDs::decayMs.toString()))
  {
    // since sustain hasn't been passed to this function, the y positions for
    // the last two points just keep their existing values
    auto range = state->getAPVTS()->getParameterRange(paramID);
    if (!range.getRange().contains(value))
    {
      DLog::log("Out of range decay value: " + String(value));
    }
    auto xDecay = (range.convertTo0to1(value) * getMaxDecayLength(fBounds)) +
                  holdEnd.getX();
    return {xDecay, point->getY()};
  } else if (paramID.contains(IDs::sustainLevel.toString()))
  {
    // since this could be one of two points, we keep the point's previous X
    // value
    auto susY = sustainLevelY(fBounds, value);
    return {point->getX(), susY};
  } else if (paramID.contains(IDs::releaseMs.toString()))
  {
    auto range = state->getAPVTS()->getParameterRange(paramID);
    if (!range.getRange().contains(value))
    {
      DLog::log("Out of range release value: " + String(value));
    }
    auto xRelease = fBounds.getRight() -
                    (range.convertTo0to1(value) * getMaxReleaseLength(fBounds));
    return {xRelease, point->getY()};
  } else if (paramID.contains(IDs::releaseCurve.toString()))
  {
    // DLog::log("Getting position from release curve value: " + String(value));
    float xPos = Math::flerp(sustainEnd.getX(), fBounds.getRight(), 0.5f);
    float yPos =
        Math::flerp(sustainEnd.getY(), fBounds.getBottom(), 1.0f - value);
    return {xPos, yPos};
  }
  DLog::log("Warning: requested position for invalid param ID: \"" + paramID +
            "\"");
  return {0.0f, 0.0f};
}

float EnvelopeGraphCore::getParamFromPos(const String &paramID,
                                         DragPoint *point, Point<float> pos)
{
  auto range = state->getAPVTS()->getParameterRange(paramID);
  auto fBounds = getLocalBounds().toFloat();
  const float yTop = fBounds.getY() + 5.0f;
  if (point == &attackEnd)
  {
    auto fAttack = (pos.x / getMaxAttackLength(fBounds));
    if (fAttack < 0.0f || fAttack > 1.0f)
    {
      DLog::log("FAttack is out of range in EnvelopeGraph.cpp!");
    }
    return range.convertFrom0to1(fAttack);

  } else if (point == &attackCurve)
  {
    float y0 = yTop;
    float y1 = fBounds.getBottom() - 5.0f;
    return (pos.y - y1) / (y0 - y1);
  } else if (point == &holdEnd)
  {
    auto fHold =
        (pos.x - (attackEnd.getX() + HOLD_BUF)) / getMaxHoldLength(fBounds);
    if (fHold < 0.0f || fHold > 1.0f)
    {
      DLog::log("fHold is out of range in EnvelopeGraph.cpp!");
    }
    return range.convertFrom0to1(fHold);
  } else if (point == &decayCurve)
  {
    float y1 = decayEnd.getY();
    float output;
    if (y1 != yTop)
    {
      output = (pos.y - y1) / (yTop - y1);
    } else
    {
      output = state->getFloatParamValue(paramID);
    }
    return output;
  } else if (point == &decayEnd) // careful: what looks like two cases is
                                 // actually four cases because the last two
                                 // points can control 1 of 2 parameters
  {
    if (paramID.contains(IDs::sustainLevel.toString())) // sustain
    {
      return sustainFromY(fBounds, pos.y);
    } else // decay time
    {
      auto fDecay = (pos.x - holdEnd.getX()) / getMaxDecayLength(fBounds);
      if (fDecay < 0.0f || fDecay > 1.0f)
      {
        DLog::log("fDecay is out of range in EnvelopeGraph.cpp!");
      }
      return range.convertFrom0to1(fDecay);
    }
  } else if (point == &sustainEnd)
  {
    if (paramID.contains(IDs::sustainLevel.toString())) // sustain
    {
      return sustainFromY(fBounds, pos.y);
    } else // release time
    {
      auto fRelease =
          (fBounds.getRight() - pos.x) / getMaxReleaseLength(fBounds);
      if (fRelease < 0.0f || fRelease > 1.0f)
      {
        DLog::log("fRelease is out of range in EnvelopeGraph.cpp!");
      }
      return range.convertFrom0to1(fRelease);
    }
  } else if (point == &releaseCurve)
  {
    // DLog::log("Calculating release value from Y position: " + String(pos.y));
    float yMax = fBounds.getBottom();
    float yMin = sustainEnd.getY();
    // DLog::log("Y range is between " + String(yMin) + " and " + String(yMax));
    return 1.0f - ((pos.y - yMin) / (yMax - yMin));
  }
  DLog::log("Warning- no point matching " +
            String(reinterpret_cast<const char *>(point)) + " was found!");
  return 0.0f;
}

Point<float> EnvelopeGraphCore::constrainPositionFor(DragPoint *point,
                                                     Point<float> pos)
{
  auto bounds = getLimitsFor(point);
  return bounds.getConstrainedPoint(pos);
}

DragPoint *EnvelopeGraphCore::getPointWithinRadius(const MouseEvent &e,
                                                   float radius)
{
  for (auto p : points)
  {
    if (p->isWithin(e, radius))
      return p;
  }
  return nullptr;
}

Rectangle<float> EnvelopeGraphCore::getLimitsFor(DragPoint *pt)
{
  auto fBounds = getLocalBounds().toFloat();
  auto yTop = fBounds.getY() + 5.0f;
  auto yBottom = fBounds.getBottom() - 5.0f;
  float yHeight = yBottom - yTop;
  if (pt == &attackEnd)
  {
    return {0.0f, yTop, getMaxAttackLength(fBounds), 0.0f};
  } else if (pt == &attackCurve)
  {
    auto cX = attackEnd.getX() / 2.0f;
    return {cX, yTop, 0.0f, yHeight};
  } else if (pt == &holdEnd)
  {
    auto xMin = attackEnd.getX() + HOLD_BUF;
    return {xMin, yTop, getMaxHoldLength(fBounds), 0.0f};
  } else if (pt == &decayCurve)
  {
    auto xMin = Math::flerp(holdEnd.getX(), decayEnd.getX(), 0.5f);
    auto range = decayEnd.getY() - yTop;
    return {xMin, yTop, 0.0f, range};
  } else if (pt == &decayEnd)
  {
    auto xMin = holdEnd.getX();
    return {xMin, yTop, getMaxDecayLength(fBounds), yHeight};
  } else if (pt == &sustainEnd)
  {
    auto width = getMaxReleaseLength(fBounds);
    return {fBounds.getRight() - width, yTop, width, yHeight};
  } else // release curve
  {
    float xMin = Math::flerp(sustainEnd.getX(), fBounds.getRight(), 0.5f);
    float range = fBounds.getHeight() - sustainEnd.getY();
    return {xMin, sustainEnd.getY(), 0.0f, range};
  }
}

// this forces all the drag points to the positions determined by the values
// from shared state
void EnvelopeGraphCore::syncWithState()
{
  String iStr(index);
  auto attackMsID = IDs::attackMs.toString() + iStr;
  auto attackCurveID = IDs::attackCurve.toString() + iStr;
  auto holdID = IDs::holdMs.toString() + iStr;
  auto decayCurveID = IDs::decayCurve.toString() + iStr;
  auto decayID = IDs::decayMs.toString() + iStr;
  auto sustainID = IDs::sustainLevel.toString() + iStr;
  auto releaseID = IDs::releaseMs.toString() + iStr;
  auto releaseCurveID = IDs::releaseCurve.toString() + iStr;

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
  if (newAtkPos != attackEnd.getPos())
  {
    // DLog::log("Updating attack position");
    attackEnd.moveTo(newAtkPos);
  }

  auto aCurvePos =
      getPosFromParam(attackCurveID, &attackCurve, attackCurveValue);
  if (aCurvePos != attackCurve.getPos())
  {
    attackCurve.moveTo(aCurvePos);
  }

  auto holdPos = getPosFromParam(holdID, &holdEnd, holdMs);
  if (holdPos != holdEnd.getPos())
  {
    // DLog::log("Updating hold position");
    holdEnd.moveTo(holdPos);
  }
  auto sustainPos = getPosFromParam(sustainID, &decayEnd, sustainLevel);
  auto decayPos = getPosFromParam(decayID, &decayEnd, decayMs);
  decayPos.y = sustainPos.y;
  if (decayPos != decayEnd.getPos()) // decay time has changed
  {
    // DLog::log("Updating decay position");
    decayEnd.moveTo(decayPos);
  }

  auto dCurvePos = getPosFromParam(decayCurveID, &decayCurve, decayCurveValue);
  if (dCurvePos != decayCurve.getPos())
  {
    // DLog::log("Moving decay curve to: " + dCurvePos.toString());
    decayCurve.moveTo(dCurvePos);
  }
  auto releasePos = getPosFromParam(releaseID, &sustainEnd, releaseMs);
  releasePos.y = sustainPos.y;
  if (releasePos != sustainEnd.getPos())
  {
    // DLog::log("Updating release position");
    sustainEnd.moveTo(releasePos);
  }
  auto releaseCurvePos =
      getPosFromParam(releaseCurveID, &releaseCurve, releaseCurveValue);
  if (releaseCurvePos != releaseCurve.getPos())
  {
    // DLog::log("Updating release curve position: " +
    // String(releaseCurvePos.toString()));
    releaseCurve.moveTo(releaseCurvePos);
  }
}

void EnvelopeGraphCore::drawHandle(Graphics &g, Point<float> center,
                                   float radius, bool fill)
{
  Rectangle<float> bounds(radius * 2.0f, radius * 2.0f);
  bounds = bounds.withCentre(center);
  g.setColour(Color::brightYellow);
  if (fill)
  {
    g.fillEllipse(bounds);
  } else
  {
    g.drawEllipse(bounds, HANDLE_STROKE);
  }
}
//=============================================================================================

EnvelopeGraph::EnvelopeGraph(EVT *tree, int idx)
    : core(tree, idx), level(tree, idx)
{
  addAndMakeVisible(&core);
  addAndMakeVisible(&level);
}

void EnvelopeGraph::resized()
{
  auto bounds = getLocalBounds();
  level.setBounds(bounds);
  core.setBounds(bounds);
  level.toBack();
}

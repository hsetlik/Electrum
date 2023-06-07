#include "EnvelopePanel.h"

//======================================================================================================
EnvelopePanel::EnvelopePanel(EVT* tree, int idx) :
state(tree),
index(idx)
{
    String iStr(index);
    auto attackID = IDs::attackMs.toString() + iStr;
    std::function<void(float)> attackChangeFunc = [this, attackID](float value)
    {
        this->dragParamChanged(attackID, value);
    };

    std::function<float(float, float)> attackMsConvert = [this, attackID](float x, float y)
    {
        return this->parameterFromPoint(attackID, x, y);
    };

    attackMsAttach.reset(new DragPointParameterAttachment(
    state, 
    &attackEnd, 
    attackID,
    attackChangeFunc,
    attackMsConvert));
    
    auto holdID = IDs::holdMs.toString() + iStr;
    std::function<void(float)> holdChangeFunc = [this, holdID](float value)
    {
        this->dragParamChanged(holdID, value);
    };

    std::function<float(float, float)> holdMsConvert = [this, holdID](float x, float y)
    {
        return this->parameterFromPoint(holdID, x, y);
    };
    holdMsAttach.reset(new DragPointParameterAttachment(
    state, 
    &holdEnd, 
    holdID,
    holdChangeFunc,
    holdMsConvert));

    auto decayID = IDs::decayMs.toString() + iStr;
    std::function<void(float)> decayChangeFunc = [this, decayID](float value)
    {
        this->dragParamChanged(decayID, value);
    };

    std::function<float(float, float)> decayMsConvert = [this, decayID](float x, float y)
    {
        return this->parameterFromPoint(decayID, x, y);
    };
    decayMsAttach.reset(new DragPointParameterAttachment(
    state, 
    &decayEnd, 
    decayID,
    decayChangeFunc,
    decayMsConvert));
 
    auto sustainID = IDs::sustainLevel.toString() + iStr;
    std::function<void(float)> sustainChangeFunc = [this, sustainID](float value)
    {
        this->dragParamChanged(sustainID, value);
    };

    std::function<float(float, float)> sustainConvert = [this, sustainID](float x, float y)
    {
        return this->parameterFromPoint(sustainID, x, y);
    };
    sustainAttach1.reset(new DragPointParameterAttachment(
    state, 
    &decayEnd, 
    sustainID,
    sustainChangeFunc,
    sustainConvert));
     
    sustainAttach2.reset(new DragPointParameterAttachment(
    state, 
    &sustainEnd, 
    sustainID,
    sustainChangeFunc,
    sustainConvert));      

    auto releaseID = IDs::releaseMs.toString() + iStr;
    std::function<void(float)> releaseChangeFunc = [this, releaseID](float value)
    {
        this->dragParamChanged(releaseID, value);
    };

    std::function<float(float, float)> releaseMsConvert = [this, releaseID](float x, float y)
    {
        return this->parameterFromPoint(releaseID, x, y);
    };
    releaseMsAttach.reset(new DragPointParameterAttachment(
    state, 
    &sustainEnd, 
    releaseID,
    releaseChangeFunc,
    releaseMsConvert));
    //=========================================================
    triggerAsyncUpdate();
}


void EnvelopePanel::paint(Graphics& g)
{
    auto bkgndColor = Color::getColorForModSource(IDs::envSource.toString() + String(index));
    g.fillAll(bkgndColor);
    auto lBounds = getLocalBounds().toFloat();
    Path p;
    p.startNewSubPath(0.0f, lBounds.getHeight());
    p.lineTo(attackEnd.getFPointWithin(this)),
    // draw the Attack handle
    drawHandle(g, this, &attackEnd, true);

    p.lineTo(holdEnd.getFPointWithin(this));
    //draw the hold handle
    drawHandle(g, this, &holdEnd, true);

    p.lineTo(decayEnd.getFPointWithin(this));
    drawHandle(g, this, &decayEnd, true);

    p.lineTo(sustainEnd.getFPointWithin(this));
    drawHandle(g, this, &sustainEnd, true);

    p.lineTo(lBounds.getWidth(), lBounds.getHeight());

    PathStrokeType pst(1.5f);
    g.strokePath(p, pst);

}

String EnvelopePanel::getPointName(DragPoint* pt)
{
    if(pt == &attackEnd)
        return "Attack End";
    else if(pt == &holdEnd)
        return "Hold End";
    else if(pt == &decayEnd)
        return "Decay End";
    else if(pt == &sustainEnd)
        return "Sustain End";
    return "Not a valid DragPoint for this envelope!";
}


DragPointParameterAttachment* EnvelopePanel::getAttachmentFor(DragPoint* pt)
{
    if(pt == &attackEnd)
        return attackMsAttach.get();
    else if(pt == &holdEnd)
        return holdMsAttach.get();
    else if(pt == &decayEnd)
        return decayMsAttach.get();
    else if(pt == &sustainEnd)
        return sustainAttach1.get();
    return nullptr;
}
void EnvelopePanel::mouseDown(const MouseEvent &event) 
{
    auto point = getPointWithin(event, 3.0f);
    if(point!= nullptr)
    {
        point->startMovement();
        selectedPoint = point;
    }
    else
    {
        DLog::log("Click was not on a drag point");
        selectedPoint = nullptr;
    }
}
void EnvelopePanel::mouseDrag(const MouseEvent &event)
{
    if(selectedPoint != nullptr)
    {
        selectedPoint->moveTo(event);
        triggerAsyncUpdate();
    }
}
void EnvelopePanel::mouseUp(const MouseEvent &)
{
    if(selectedPoint != nullptr)
    {
        selectedPoint->endMovement();
    }
    selectedPoint = nullptr;
    triggerAsyncUpdate();
}

void EnvelopePanel::handleAsyncUpdate()
{
    syncWithState();
    repaint();
}

void EnvelopePanel::syncWithState()
{
    //grip the updated values
    const float attackMs = state->getFloatParamValue(IDs::attackMs.toString() + String(index));
    const float holdMs = state->getFloatParamValue(IDs::holdMs.toString() + String(index));
    const float decayMs = state->getFloatParamValue(IDs::decayMs.toString() + String(index));
    const float sustainLevel = state->getFloatParamValue(IDs::sustainLevel.toString() + String(index));
    const float releaseMs = state->getFloatParamValue(IDs::releaseMs.toString() + String(index));
    // move each drag point appropriately
    float fAttack = attackMs / ENV_MS_MAX;
    attackEnd.setXRange(0.0f, ATTACK_MS_MAX / ENV_MS_MAX);
    attackEnd.setPoint(fAttack, 0.0f, false);
    // now the hold range needs to change
    holdEnd.setXRange(fAttack, fAttack + (HOLD_MS_MAX / ENV_MS_MAX));
    float fHold = fAttack + (holdMs / ENV_MS_MAX);
    holdEnd.setPoint(fHold, 0.0f, false);
    // now decay range needs to change
    decayEnd.setXRange(fHold, fHold + (DECAY_MS_MAX / ENV_MS_MAX));
    decayEnd.setYRange(0.0f, 1.0f);
    float fDecay = fHold + (decayMs / ENV_MS_MAX);
    decayEnd.setPoint(fDecay, 1.0f - sustainLevel, false);
    // and change release range
    float fReleaseMax = RELEASE_MS_MAX / ENV_MS_MAX;
    sustainEnd.setXRange(1.0f - fReleaseMax, 1.0f);
    sustainEnd.setYRange(0.0f, 1.0f);
    float fRelease = 1.0f - (releaseMs / ENV_MS_MAX);
    sustainEnd.setPoint(fRelease, 1.0f - sustainLevel, false);

}

void EnvelopePanel::drawHandle(Graphics&g, Component* parent, DragPoint* point, bool shouldBeFilled)
{
    const float radius = 3.0f;
    auto center = point->getFPointWithin(parent);
    Rectangle<float> bounds(radius * 2.0f, radius * 2.0f);
    bounds = bounds.withCentre(center);
    g.setColour(Color::indigo);
    if (shouldBeFilled)
        g.fillEllipse(bounds);
    else
        g.drawEllipse(bounds, 1.8f);

}
//=========================================================================
//thought: both of these can basically just pattern match
void EnvelopePanel::dragParamChanged(const String& id, float newValue)
{
    String iStr(index);
    auto range = state->getAPVTS()->getParameterRange(id);
    if(id == IDs::attackMs.toString() + iStr)
    {
        auto fNormalized = range.convertTo0to1(newValue) * (ATTACK_MS_MAX / ENV_MS_MAX);
        if (fNormalized != attackEnd.getX())
        {
            attackEnd.setPoint(fNormalized, attackEnd.getY());
        }
    }
    else if(id == IDs::holdMs.toString() + iStr)
    {
        auto fNormalized = (range.convertTo0to1(newValue) * (HOLD_MS_MAX / ENV_MS_MAX)) + attackEnd.getX();
        if(fNormalized != holdEnd.getX())
        {
            holdEnd.setPoint(fNormalized, holdEnd.getY());
        }

    }
    else if(id == IDs::decayMs.toString() + iStr)
    {
        auto fNormalized = (range.convertTo0to1(newValue) * (DECAY_MS_MAX / ENV_MS_MAX)) + holdEnd.getX();
        if(fNormalized != decayEnd.getX())
        {
            decayEnd.setPoint(fNormalized, decayEnd.getY());
        }
    }
    else if(id == IDs::sustainLevel.toString() + iStr)
    {
        // remember: we need to set both decayEnd and sustainEnd on the Y axis
        float val = 1.0f - newValue;
        if(decayEnd.getY() != val)
        {
            decayEnd.setPoint(decayEnd.getX(), val);
        }
        if(sustainEnd.getY() != val)
        {
            sustainEnd.setPoint(sustainEnd.getX(), val);
        }
    }
    else if(id == IDs::releaseMs.toString() + iStr)
    {
        float fNormalized = 1.0f - (range.convertTo0to1(newValue) * (RELEASE_MS_MAX / ENV_MS_MAX));
        if(sustainEnd.getX() != fNormalized)
        {
            sustainEnd.setPoint(fNormalized, sustainEnd.getY());
        }
    }
}

float EnvelopePanel::parameterFromPoint(const String& id, float x, float y)
{
    String iStr(index);
    auto range = state->getAPVTS()->getParameterRange(id);
    if(id == IDs::attackMs.toString() + iStr)
    {
        auto normalizedPoint = x / (ATTACK_MS_MAX / ENV_MS_MAX);
        return range.convertFrom0to1(normalizedPoint);
    }
    else if(id == IDs::holdMs.toString() + iStr)
    {
        // we need to take the offset of the beginning of the hold range based on the current attack
        auto xOffset = attackEnd.getX();
        auto normalized = (x - xOffset) / (HOLD_MS_MAX / ENV_MS_MAX);
        return range.convertFrom0to1(normalized);
    }
    else if(id == IDs::decayMs.toString() + iStr)
    {
        auto xOffset = holdEnd.getX();
        auto normalized = (x - xOffset) / (DECAY_MS_MAX / ENV_MS_MAX);
        return range.convertFrom0to1(normalized);
    }
    else if(id == IDs::sustainLevel.toString() + iStr)
    {
        // easy!
        return 1.0f - y;
    }
    else if(id == IDs::releaseMs.toString() + iStr)
    {
        float normalized = (1.0f - x) / (RELEASE_MS_MAX / ENV_MS_MAX);
        return range.convertFrom0to1(normalized);
    }
    return 0.0f;
}


DragPoint* EnvelopePanel::getPointWithin(const MouseEvent& e, float radius)
{
    for(auto p : points)
    {
        auto center = p->getFPointWithin(e.eventComponent);
        auto distance = center.getDistanceFrom(e.position);
        if(distance < radius)
            return p;
    }
    return nullptr;
}
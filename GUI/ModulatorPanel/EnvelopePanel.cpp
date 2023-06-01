#include "EnvelopePanel.h"

//======================================================================================================
EnvelopePanel::EnvelopePanel(EVT* tree, int idx) :
state(tree),
index(idx),
attackEnd(0.0025f, 0.0f, {0.0f, 0.25f}, {0.0f, 0.0f}),
holdEnd(0.0025f, 0.0f, {0.0f, 0.25f}, {0.0f, 0.0f}),
{
    attackEnd.addListener(this);
    holdEnd.addListener(this);
    decayEnd.addListener(this);
    sustainEnd.addListener(this);
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
void EnvelopePanel::mouseDown(const MouseEvent &event) 
{
    for(auto p : points)
    {
        if (p->isWithin(event, 3.0f));
        {
            selectedPoint = p;
            break;
        }
    }
}
void EnvelopePanel::mouseDrag(const MouseEvent &event)
{
    if(selectedPoint != nullptr)
    {
        selectedPoint->moveTo(event);
    }
}
void EnvelopePanel::mouseUp(const MouseEvent &)
{
    selectedPoint = nullptr;
}


void EnvelopePanel::pointMoved(DragPoint* point, float newX, float newY)
{
    //TODO handle ParameterAttachment updating here

    repaint();
}

void EnvelopePanel::syncWithState()
{

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
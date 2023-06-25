#include "DragPoint.h"

DragPointAttachment::DragPointAttachment(EVT* tree, const String& param, DragPoint* p, PosToParamFunc func1, ParamToPosFunc func2) :
state(tree),
paramID(param),
point(p),
posToParam(func1),
paramToPos(func2)
{
  point->addListener(this);
  auto callback = [this](float value)
  {
    auto pos = paramToPos(value);
    point->movePoint(pos.x, pos.y, false);
  };
  auto treeParam = state->getAPVTS()->getParameter(paramID);
  pAttach.reset(new ParameterAttachment(*treeParam, callback, nullptr));
}

DragPointAttachment::~DragPointAttachment()
{
  point->removeListener(this);
}

void DragPointAttachment::moveStarted(DragPoint* )
{
  pAttach->beginGesture();
}

void DragPointAttachment::moveEnded(DragPoint*)
{
  pAttach->endGesture();
}

void DragPointAttachment::moved(DragPoint* p)
{
  auto val = posToParam({p->getX(), p->getY()});
  pAttach->setValueAsPartOfGesture(val);
}

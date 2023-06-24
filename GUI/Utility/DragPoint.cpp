#include "DragPoint.h"

DragPointAttachment::DragPointAttachment(EVT* tree, const String& param, DragPoint* p, PosToParamFunc func1, ParamToPosFunc func2) :
state(tree),
paramID(param),
point(p),
posToParam(func1),
paramToPos(func2)
{
  point->addListener(this);
}

DragPointAttachment::~DragPointAttachment()
{
  point->removeListener(this);
}

void DragPointAttachment::moveStarted(DragPoint* p)
{

}

void DragPointAttachment::moveEnded(DragPoint* p)
{

}

void DragPointAttachment::moved(DragPoint* p)
{

}

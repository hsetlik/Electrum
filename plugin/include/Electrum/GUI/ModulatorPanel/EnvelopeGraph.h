#pragma once
#include "../Util/DragPoint.h"
#include "Electrum/GUI/GUITypedefs.h"
#include "juce_events/juce_events.h"

#define FATTACK 0.2f
#define FHOLD 0.1f
#define FDECAY 0.3f
#define FRELEASE 0.4f

#define HANDLE_STROKE 0.75f
#define HOLD_BUF 4.0f

class EnvelopeGraph : public Component, public juce::AsyncUpdater {
private:
  ElectrumState* const state;
  void drawEnvelopeGraph(frect_t& bounds, juce::Graphics& g);

public:
  const int envID;
  EnvelopeGraph(ElectrumState* s, int id);
  void paint(juce::Graphics& g) override;
  void handleAsyncUpdate() override;
  void mouseDown(const juce::MouseEvent& e) override;
  void mouseDrag(const juce::MouseEvent& e) override;
  void mouseUp(const juce::MouseEvent& e) override;
  // make sure things are synced with the state when the component is brought
  // into focus
  void broughtToFront() override { triggerAsyncUpdate(); }

private:
  // DragPoints/attachments
  DragPoint attackEnd;
  drag_attach_ptr attackMsAttach;

  DragPoint attackCurve;
  drag_attach_ptr attackCurveAttach;

  DragPoint holdEnd;
  drag_attach_ptr holdMsAttach;

  DragPoint decayCurve;
  drag_attach_ptr decayCurveAttach;

  DragPoint decayEnd;
  drag_attach_ptr sustainAttach1;
  drag_attach_ptr decayMsAttach;

  DragPoint sustainEnd;
  drag_attach_ptr sustainAttach2;
  drag_attach_ptr releaseMsAttach;

  DragPoint releaseCurve;
  drag_attach_ptr releaseCurveAttach;

  // handy for iterating over all the points
  // NOTE: these are in left to right order
  const std::array<DragPoint*, 7> points = {
      &attackCurve, &attackEnd,  &holdEnd,     &decayCurve,
      &decayEnd,    &sustainEnd, &releaseCurve};
  //------------------------
  DragPoint* selectedPoint = nullptr;
  bool isMoving = false;
  // these make up the attachment lambdas
  fpoint_t getPosFromParam(const String& paramID,
                           DragPoint* point,
                           float value);
  float getParamFromPos(const String& paramID, DragPoint* point, fpoint_t pos);
  // positioning/constraining logic
  fpoint_t constrainPositionFor(DragPoint* dp, fpoint_t pos);
  frect_t getLimitsFor(DragPoint* dp);

  void syncWithState();
};

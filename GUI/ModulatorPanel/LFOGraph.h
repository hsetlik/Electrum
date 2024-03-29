#pragma once
#include "../Modulation/LabeledSPS.h"
#include "../Modulation/ModulationSourceComponent.h"
#include "EnvelopeGraph.h"

class LFOGraph : public Component, public AsyncUpdater
{
private:
  void drawLFOGraph(Rectangle<float> &bounds, Graphics &g);
  EVT *const state;

public:
  const int index;
  LFOGraph(EVT *tree, int i);
  void paint(Graphics &g) override;
  void handleAsyncUpdate() override;
  // mouse callbacks
  void mouseDown(const MouseEvent &e) override;
  void mouseDrag(const MouseEvent &e) override;
  void mouseUp(const MouseEvent &e) override;
  // make sure things are synced with the state when the component is brought into focus
  void broughtToFront() override { triggerAsyncUpdate(); }

private:
  DragPoint curveA;
  std::unique_ptr<DragPointAttachment> curveAAttach;

  DragPoint center;
  std::unique_ptr<DragPointAttachment> centerAttach;

  DragPoint curveB;
  std::unique_ptr<DragPointAttachment> curveBAttach;

  DragPoint *selectedPoint;
  bool isMoving;
  DragPoint *getPointWithinRadius(const MouseEvent &e, float radius);
  const std::vector<DragPoint *> points = {&curveA, &center, &curveB};
  // these get called from the attachment callback
  Point<float> getPosFromParam(const String &paramID, DragPoint *point, float value);
  float getParamFromPos(const String &paramID, DragPoint *point, Point<float> pos);
  Point<float> constrainPositionFor(DragPoint *point, Point<float> pos);
  // helper function, get the max limits within this component of a given DragPoint
  Rectangle<float> getLimitsFor(DragPoint *pt);
  // this grips values from the APVTS and makes sure the points match them
  void syncWithState();
  // helper for drawing handles
  static void drawHandle(Graphics &g, Point<float> center, float radius, bool fill);
};
//=======================================================================================
class LFOLevelComponent : public Component, public Timer
{
public:
  EVT *const state;
  const int index;
  LFOLevelComponent(EVT *tree, int idx);
  void timerCallback() override;
  void paint(Graphics &g) override;

private:
  Colour upper;
  Colour lower;
  float lastLevel;
};
//=======================================================================================
class LFOPanel : public Component
{
private:
  EVT *const state;
  const int index;
  LFOGraph graph;
  LFOLevelComponent level;
  LabeledSPS freqSlider;
  ModulationSourceComponent srcComp;
  ComboBox bTriggerMode;
  std::unique_ptr<APVTS::ComboBoxAttachment> triggerModeAttach;

public:
  LFOPanel(EVT *tree, int i);
  void resized() override;
};

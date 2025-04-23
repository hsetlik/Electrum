#pragma once

#include "../GUITypedefs.h"
#include "Electrum/Audio/Modulator/LFO.h"
#include "Electrum/Identifiers.h"
#include "Electrum/Shared/ElectrumState.h"

// namespace for our identifiers
namespace LFOID {
#define DECLARE_ID(name) const juce::Identifier name(#name);

// Top level tree
DECLARE_ID(LFO_EDIT_STATE)
DECLARE_ID(lfoIndex)
// child tree to represent each handle
DECLARE_ID(LFO_HANDLE_STATE)
DECLARE_ID(handleLevel)
DECLARE_ID(handleTableIndex)
DECLARE_ID(handleIndex)
DECLARE_ID(handleIsLocked)

#undef DECLARE_ID

// parse the string-encoded LFO shape into a ValueTree for
// track the editor's state
ValueTree getLFOEditorTree(ElectrumState* s, int idx);

}  // namespace LFOID

// this is just a 'lfo_handle_t' that also has handleIndex information and
// can be sorted. I suspect this is a bit faster/easier on the message thread
// than doing everything via ValueTree;
struct edit_handle_t {
  int tableIdx;
  float level;
  bool locked;
  // this can run in the initializer
  static std::vector<edit_handle_t> parseValueTree(const ValueTree& tree);
};

// this holds onto our editing state, our main click
// click viewport editor will own one.
class LFOEditState {
private:
  ValueTree editState;
  std::vector<edit_handle_t> handles;
  int indexOf(edit_handle_t* handle) const;

public:
  const int lfoID;
  LFOEditState(ElectrumState* s, int idx);

  // Validation for adding/moving/deleting handles---------------------------
  bool handleExistsAt(int tableIdx) const;
  bool handleIsLocked(int handleIndex) const;
  bool handleCanMoveTo(int handleIdx, int tableIdx) const;
  bool handleCanMoveTo(edit_handle_t* ptr, int tableIdx) const;
  int handleIdxToLeft(int tableIdx) const;
  bool canCreateHandle(int tableIdx, float lvl) const;

  // Mode/create/delete LFO points
  // creates a handle at the specified tableIdx and level. returns
  // the new handle's handle index, or -1 if unsuccessful
  int createHandle(int tableIdx, float level);
  // removes the handle at the given handle index, returns success or failure
  bool removeHandle(int handleIdx);

  // returns the handle index of the handle at the given table index, or -1 if
  // none exists
  int handleIndexAt(int tableIndex) const;

  // Mouse handling stuff-----------------------------------
  void processMouseDown(const frect_t& bounds, const juce::MouseEvent& me);
  void processMouseDrag(const frect_t& bounds, const juce::MouseEvent& me);
  void processMouseUp(const frect_t& bounds, const juce::MouseEvent& me);
  void processDoubleClick(const frect_t& bounds, const juce::MouseEvent& me);

  // buttons/keystrokes should call these as appropriate-----
  void deleteSelectedHandles();
  // Drawing callback---------------------------------------
  void drawSection(juce::Graphics& g,
                   const frect_t& bounds,
                   float xNormStart,
                   float xNormEnd) const;

  bool shouldRedraw() const { return needsRedraw.load(); }

private:
  // help us not redraw 5 gajillion times
  std::atomic<bool> needsRedraw;
  // clicking/dragging helpers
  bool mouseIsDown = false;
  bool isDraggingSelection = false;
  lfo_handle_t lastMouseDownHandlePos;
  lfo_handle_t lastDragUpdateHandlePos;
  // checks if the current relative movement is legal for the current selection
  bool dragMovementIsLegal(const frect_t& bounds,
                           const fpoint_t& startPt,
                           const fpoint_t& endPt) const;
  void dragCurrentSelection(const frect_t& bounds,
                            const fpoint_t& startPt,
                            const fpoint_t& endPt);
  // returns the handleIndex of the handle closest to the given point
  int findClosestEditHandle(const frect_t& bounds, const fpoint_t& point);
  // returns a pointer to the edit handle within the given distance or nullptr
  // if there is none
  edit_handle_t* getHandleWithinDist(const frect_t& bounds,
                                     const fpoint_t& point,
                                     float thresh = 5.0f);

  // selection handling stuff-----------------------------------
  edit_handle_t* lastSelectedHandle = nullptr;
  bool isSelected(const edit_handle_t* hand) const;
  void selectHandle(edit_handle_t* hand);
  void deselectHandle(edit_handle_t* hand);
  void clearSelection() {
    lastSelectedHandle = nullptr;
    selectedHandles.clear();
  }
  std::vector<edit_handle_t*> selectedHandles;
  // helper for lasso multi-selection
  void loadLassoIntoSelection(const frect_t& bounds,
                              const fpoint_t& cornerA,
                              const fpoint_t& cornerB);

  // drawing helpers-------------------------------------------
  int firstHandleToDraw(float xStart) const;
  int lastHandleToDraw(float xEnd) const;
  void drawBackground(juce::Graphics& g,
                      const frect_t& bounds,
                      float xNormStart,
                      float xNormEnd) const;
  void drawShape(juce::Graphics& g,
                 const frect_t& bounds,
                 float xNormStart,
                 float xNormEnd) const;
  void drawHandles(juce::Graphics& g,
                   const frect_t& bounds,
                   float xNormStart,
                   float xNormEnd) const;
  //-----------------------------------------------------------

  // we do a little geometry/projection
  static fpoint_t projectHandleToPoint(const frect_t& bounds,
                                       const lfo_handle_t& handle);
  static fpoint_t projectHandleToPoint(const frect_t& bounds,
                                       const edit_handle_t& handle);
  static lfo_handle_t projectPointToHandle(const frect_t& bounds,
                                           const fpoint_t& point);
};

//============================================================
//--------------COMPONENTS------------------------------------
//============================================================
class ViewedLFOEditor : public Component, public juce::Timer {
private:
  LFOEditState editState;

public:
  ViewedLFOEditor(ElectrumState* s, int idx);
  void resized() override;
  void paint(juce::Graphics& g) override;
  void timerCallback() override;
  void setWidthForScale(float normScale);

  void mouseDown(const juce::MouseEvent& me) override {
    editState.processMouseDown(getLocalBounds().toFloat(), me);
  }
  void mouseUp(const juce::MouseEvent& me) override {
    editState.processMouseUp(getLocalBounds().toFloat(), me);
  }
  void mouseDrag(const juce::MouseEvent& me) override {
    editState.processMouseDrag(getLocalBounds().toFloat(), me);
  }
  void mouseDoubleClick(const juce::MouseEvent& me) override {
    editState.processDoubleClick(getLocalBounds().toFloat(), me);
  }
};

// This is the main modal component that holds our editor
class LFOEditor : public Component {
private:
  ElectrumState* const state;
  const int lfoID;
  // main editor
  ViewedLFOEditor editor;
  juce::Viewport vpt;

  // save/close buttons
  juce::TextButton saveButton;
  juce::TextButton closeButton;

  // zoom scaling slider/label
  juce::Slider zoomSlider;
  BoundedAttString zoomStr;

public:
  LFOEditor(ElectrumState* s, int idx);
  ~LFOEditor() override;
  void resized() override;
  void paint(juce::Graphics& g) override;
};

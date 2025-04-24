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
  static std::vector<edit_handle_t> parseShapeString(const String& str);
};

// this holds onto our editing state, our main click
// click viewport editor will own one.
class LFOEditState {
private:
  std::vector<edit_handle_t> handles;
  int indexOf(edit_handle_t* handle) const;

public:
  LFOEditState(ElectrumState* s, int idx);
  LFOEditState(const String& shapeStr);

  // Validation for adding/moving/deleting handles---------------------------
  bool handleExistsAt(int tableIdx) const;
  bool handleIsLocked(int handleIndex) const;
  bool handleCanMoveTo(int handleIdx, int tableIdx) const;
  bool handleCanMoveTo(edit_handle_t* ptr, int tableIdx) const;
  int handleIdxToLeft(int tableIdx) const;
  bool canCreateHandle(int tableIdx, float lvl) const;
  // This gets the editor's state as a string to sent to a `LowFrequencyLUT`
  // object
  String encodeCurrentShapeString() const;
  //-------------------------------------------

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

  bool shouldRedraw() const { return needsRedraw; }
  void redrawFinished() { needsRedraw = false; }
  void requestRedraw() { needsRedraw = true; }

private:
  // help us not redraw 5 gajillion times
  bool needsRedraw;
  // clicking/dragging helpers
  bool mouseIsDown = false;
  bool isDraggingSelection = false;
  lfo_handle_t lastMouseDownHandlePos;
  lfo_handle_t lastDragUpdateHandlePos;

  // lasso-drawing logic
  bool shouldDrawLasso = false;

  bool dragMovementIsLegal(const frect_t& bounds,
                           const fpoint_t& startPt,
                           const fpoint_t& endPt) const;
  void dragCurrentSelection(const frect_t& bounds,
                            const fpoint_t& startPt,
                            const fpoint_t& endPt);
  void dragSinglePoint(const frect_t& bounds, const fpoint_t& point);
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
  void drawLasso(juce::Graphics& g, const frect_t& bounds) const;
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
  bool viewportChanged = false;
  std::unique_ptr<LFOEditState> eState;
  uint32_t lastShapeUpdateMs = 0;
  void checkShapeUpdate();

public:
  ViewedLFOEditor(ElectrumState* s, int idx);
  void resized() override;
  void paint(juce::Graphics& g) override;
  void timerCallback() override;
  void setWidthForScale(float normScale);

  String getCurrentShapeString() const {
    return eState->encodeCurrentShapeString();
  }

  void replaceStateStr(const String& s) { eState.reset(new LFOEditState(s)); }

  void mouseDown(const juce::MouseEvent& me) override {
    eState->processMouseDown(getLocalBounds().toFloat(), me);
  }
  void mouseUp(const juce::MouseEvent& me) override {
    eState->processMouseUp(getLocalBounds().toFloat(), me);
  }
  void mouseDrag(const juce::MouseEvent& me) override {
    eState->processMouseDrag(getLocalBounds().toFloat(), me);
  }
  void mouseDoubleClick(const juce::MouseEvent& me) override {
    eState->processDoubleClick(getLocalBounds().toFloat(), me);
  }
};

//------------------------------------
enum BasicShapeE { Sine, RisingRamp, FallingRamp, Triangle, Random };

const juce::StringArray BasicShapeNames = {
    "Sine", "Rising Ramp", "Falling Ramp", "Triangle", "Random"};

class BasicShapeMenu : public Component {
private:
  BoundedAttString headerLabel;

  BoundedAttString shapeLabel;
  juce::ComboBox shapeBox;

  BoundedAttString pointsLabel;
  juce::Slider pointsSlider;

  juce::TextButton okButton;
  // callback for keeping the slider in range
  void updateSliderRange(BasicShapeE tShape);
  // callback for the ok button;
  void loadSelectionToEditor();
  std::vector<lfo_handle_t> getHandlesForSelection();

public:
  BasicShapeMenu();
  void resized() override;
  void paint(juce::Graphics& g) override;
};

//-----------------------------------

// This is the main modal component that holds our editor
class LFOEditor : public Component {
private:
  ElectrumState* const state;
  const int lfoID;
  bool previewEditorShape = false;
  // main editor
  ViewedLFOEditor editor;
  juce::Viewport vpt;

  // save/close buttons
  juce::TextButton saveButton;
  juce::TextButton closeButton;

  // side panel
  BasicShapeMenu basicMenu;

  class PreviewBtn : public juce::Button {
    AttString aStr;

  public:
    PreviewBtn();
    void paintButton(juce::Graphics& g, bool, bool) override;
  };
  PreviewBtn previewBtn;
  String savedShapeString;
  // preview button callback
  void previewBtnClicked();
  // zoom scaling slider/label
  juce::Slider zoomSlider;
  BoundedAttString zoomStr;

public:
  LFOEditor(ElectrumState* s, int idx);
  ~LFOEditor() override;
  void resized() override;
  void paint(juce::Graphics& g) override;
  // children can call this to check if they need to send data to the
  // shared state
  bool wantsPreviews() const { return previewEditorShape; }
  // the editor can call this to update the shared state's shape string
  void pushShapeString(const String& str);
  // other children can call this to send a new wave to the editor
  void replaceEditorShapeString(const String& str) {
    editor.replaceStateStr(str);
    resized();
  }
};

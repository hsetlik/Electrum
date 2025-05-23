#pragma once

#include "Electrum/Audio/AudioUtil.h"
#include "Electrum/GUI/GUITypedefs.h"
#include "Electrum/Identifiers.h"
// the different types of wave point we can have
enum WavePtType { Linear, BezierLocked, BezierFree };

// represents a a parametric point on the wave
struct wave_point_t {
  int waveIdx;
  float level;
  bool xAxisLocked = false;
  int pointType = 0;
  float leftBezLength = 0.0f;
  float leftBezTheta = 0.0f;
  float rightBezLength = 0.0f;
  float rightBezTheta = 0.0f;
};

typedef std::vector<wave_point_t> wave_pt_vec;

struct bez_handle_t {
  wave_point_t* parent = nullptr;
  bool isLeft = true;
};

struct bez_params_t {
  float normLength;
  float theta;
};

namespace Pointwise {
fpoint_t projectWavePointToSpace(const frect_t& bounds,
                                 const wave_point_t& point);
fpoint_t projectBezierHandleToSpace(const frect_t& bounds,
                                    const bez_handle_t& point);

bez_params_t projectSpaceToBezierHandle(const frect_t& bounds,
                                        const fpoint_t& center,
                                        const fpoint_t& handle);
wave_point_t projectSpaceToWavePoint(const frect_t& bounds,
                                     const fpoint_t& point);

//---------------------------------------
// for parsing to/from wave frame tree
ValueTree wavePointToTree(const wave_point_t& point);
wave_point_t treeToWavePoint(const ValueTree& tree);
ValueTree wavePointsToValueTree(const wave_pt_vec& points);
wave_pt_vec valueTreeToWavePoints(const ValueTree& warpTree);
//---------------------------------------
wave_pt_vec parseWaveLinear(float* wave);
void parseWaveLinear(float* wave, wave_pt_vec& dest);

// manages and edits the state of a single pointwise wave
class Warp {
private:
  float loadedWave[TABLE_SIZE];
  wave_pt_vec points;
  void sortPoints();
  int indexOf(wave_point_t* pt) const;

  // binary search to get the nearest point to a given waveIdx
  size_t closestPointIndex(int waveIdx) const;
  size_t leftNeighborPointIndex(int waveIdx) const;

public:
  Warp(const String& frameStr);
  Warp(const ValueTree& frameTree);
  // point validation -----------------------------------------------
  bool pointExistsAt(int waveIdx) const;
  bool canCreatePoint(int waveIdx, float lvl) const;
  bool pointCanMoveTo(wave_point_t* pt, int waveIdx, float lvl) const;

  // encode the editor contents back to a wave string----------------
  String encodeFrameString() const;
  // parse the pointwise wave into a valueTree-----------------------

  // create, delete, move single points------------------------------
  size_t createPoint(int waveIdx, float lvl, int wType = 0);
  bool deletePoint(size_t pointIdx);
  void movePointConstrained(wave_point_t* pt, int waveIdx, float lvl);
  // mouse handling-------------------------------------------------
  void processMouseDown(const frect_t& bounds, const juce::MouseEvent& me);
  void processMouseDrag(const frect_t& bounds, const juce::MouseEvent& me);
  void processMouseUp(const frect_t& bounds, const juce::MouseEvent& me);
  void processDoubleClick(const frect_t& bounds, const juce::MouseEvent& me);
  // drawing-------------------------------------------------------
  void drawSection(juce::Graphics& g,
                   const frect_t& bounds,
                   float startNorm,
                   float endNorm);

private:
  // Selection stuff--------------------------
  wave_point_t* lastSelectedPt = nullptr;
  std::vector<wave_point_t*> selectedPoints;
  bool isPointSelected(const wave_point_t* pt) const;
  void selectPoint(wave_point_t* pt);
  void deselectPoint(wave_point_t* pt);
  void clearSelection();

  wave_point_t* getNearbyPoint(const frect_t& bounds,
                               const fpoint_t& point,
                               float thresh = 4.0f);
  bez_handle_t getNearbyBezHandle(const frect_t& bounds,
                                  const fpoint_t& point,
                                  float thresh = 4.0f);

  // handle dragging a the current selection
  bool dragUpdateAllowed(const frect_t& fBounds,
                         const fpoint_t& newPoint) const;
  bool bezierDragAllowed(const frect_t& fBounds,
                         const fpoint_t& newPoint) const;
  bool bezierCanMoveTo(const frect_t& bounds,
                       const bez_handle_t& handle,
                       float length,
                       float angle) const;
  void attemptMultiPointDrag(const wave_point_t& newPoint);
  void attemptDragUpdate(const frect_t& bounds, const fpoint_t& newPoint);
  void attemptBezierDrag(const frect_t& bounds, const fpoint_t& newPoint);

  // mouse logic--------------------------
  wave_point_t lastMouseDownPoint;
  wave_point_t lastDragUpdatePoint;
  bool mouseIsDown = false;
  bool downOnSelection = false;
  bool downWithAlt = false;
  bool shouldDrawLasso = false;

  // bezier logic------------------------
  bool downOnBezier = false;
  bez_handle_t selectedBez;
  // returns the minimum waveIdx this point's right neighbor can have,
  // taking bezier handles into account
  int maxWaveIdxControlled(const wave_point_t* point) const;
  int minWaveIdxControlled(const wave_point_t* point) const;
  void advancePointType(wave_point_t* pt);

  // drawing stuff------------------------------
  float prevStartNorm = -50.0f;
  float prevEndNorm = -50.0f;
  bool waveTouched = false;
  void drawAllParts(juce::Graphics& g,
                    const frect_t& bounds,
                    float startNorm,
                    float endNorm) const;
  void drawBackground(juce::Graphics& g,
                      const frect_t& bounds,
                      float startNorm,
                      float endNorm) const;
  void drawWave(juce::Graphics& g,
                const frect_t& bounds,
                float startNorm,
                float endNorm) const;
  void drawPointHandles(juce::Graphics& g,
                        const frect_t& bounds,
                        float startNorm,
                        float endNorm) const;
  void drawWavePoint(juce::Graphics& g,
                     const frect_t& bounds,
                     const wave_point_t* pt) const;
  void drawLasso(juce::Graphics& g, const frect_t& lassoArea) const;
};

}  // namespace Pointwise

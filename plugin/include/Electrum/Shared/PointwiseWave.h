#pragma once

#include "Electrum/Audio/AudioUtil.h"
#include "Electrum/GUI/GUITypedefs.h"
#include "Electrum/Identifiers.h"

// represents a a parametric point on the wave
struct wave_point_t {
  int waveIdx;
  float level;
  bool xAxisLocked = false;
  // TODO: this will eventually be an enum w different types of points
  int pointType = 0;
};
typedef std::vector<wave_point_t> wave_pt_vec;

namespace Pointwise {
fpoint_t projectWavePointToSpace(const frect_t& bounds,
                                 const wave_point_t& point);
wave_point_t projectSpaceToWavePoint(const frect_t& bounds,
                                     const fpoint_t& point);
// TODO: maybe valuetree stuff up here eventually idk

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
  // point validation -----------------------------------------------
  bool pointExistsAt(int waveIdx) const;
  bool canCreatePoint(int waveIdx, float lvl) const;
  bool pointCanMoveTo(wave_point_t* pt, int waveIdx, float lvl) const;
  // encode the editor contents back to a wave string----------------
  String encodeFrameString() const;

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

  // handle dragging a the current selection
  bool dragUpdateAllowed(const wave_point_t& newPoint) const;
  bool dragUpdateAllowed(const frect_t& fBounds,
                         const fpoint_t& newPoint) const;
  void attemptMultiPointDrag(const wave_point_t& newPoint);
  void attemptDragUpdate(const frect_t& bounds, const fpoint_t& newPoint);
  // mouse logic--------------------------
  wave_point_t lastMouseDownPoint;
  wave_point_t lastDragUpdatePoint;
  bool mouseIsDown = false;
  bool downOnSelection = false;
  bool shouldDrawLasso = false;

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
  void drawLasso(juce::Graphics& g, const frect_t& lassoArea) const;
};

}  // namespace Pointwise

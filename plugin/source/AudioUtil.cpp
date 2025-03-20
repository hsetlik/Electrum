#include "Electrum/Audio/AudioUtil.h"
#include <cmath>
#include "Electrum/Common.h"

namespace AudioUtil {

int fastFloor32(float fValue) {
  return 32768 - (int)(32768. - fValue);
}
size_t fastFloor64(float fValue) {
  return (size_t)fastFloor32(fValue);
}

#define FAST_SINE_POINTS 2048
static std::array<float, FAST_SINE_POINTS> generateSine() {
  std::array<float, FAST_SINE_POINTS> arr;
  const float dPhase = twoPi_f / (float)FAST_SINE_POINTS;
  float phase = 0.0f;
  for (size_t i = 0; i < FAST_SINE_POINTS; ++i) {
    arr[i] = std::sinf(phase);
    phase += dPhase;
  }
  return arr;
}
//---------------------------------------------------

static std::array<float, FAST_SINE_POINTS> sinePoints = generateSine();
float fastSine(float phaseNorm) {
  return sinePoints[fastFloor64(phaseNorm * 2048.0f)];
}

// Tuning stuff
// pre-computing some LUTs for frequency/tuning math
#define SEMITONE_RATIO 1.05946309436
#define TUNING_CURVE_POINTS 4096
#define FNOTE_MIN 20.0f
#define FNOTE_MAX 109.0f

static std::array<double, TUNING_CURVE_POINTS> _generateHzCurve() {
  constexpr double fNoteDelt =
      (double)(FNOTE_MAX - FNOTE_MIN) / (double)TUNING_CURVE_POINTS;
  std::array<double, TUNING_CURVE_POINTS> arr;
  double fNote = (double)FNOTE_MIN;
  for (size_t i = 0; i < TUNING_CURVE_POINTS; ++i) {
    arr[i] = 440.0 * std::pow(SEMITONE_RATIO, fNote - 69.0);
    fNote += fNoteDelt;
  }
  return arr;
}

static std::array<double, TUNING_CURVE_POINTS> _hzCurveDouble =
    _generateHzCurve();
static std::array<float, TUNING_CURVE_POINTS> _phaseDeltCurve = {};

void updateTuningTables(double sampleRate) {
  for (size_t i = 0; i < TUNING_CURVE_POINTS; ++i) {
    _phaseDeltCurve[i] = (float)(_hzCurveDouble[i] / sampleRate);
  }
}
static size_t _idxForNote(int note, float semis, float cents) {
  constexpr float fNoteRange = (float)(FNOTE_MAX - FNOTE_MIN);
  constexpr float fIdxMax = (float)(TUNING_CURVE_POINTS - 1);
  float fNote =
      std::clamp((float)note + semis + (cents / 100.0f), FNOTE_MIN, FNOTE_MAX);
  return fastFloor64(((fNote - FNOTE_MIN) / fNoteRange) * fIdxMax);
}

float phaseDeltForNote(int midiNote, float coarseSemis, float fineCents) {
  return _phaseDeltCurve[_idxForNote(midiNote, coarseSemis, fineCents)];
}
}  // namespace AudioUtil
//===================================================

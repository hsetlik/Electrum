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

//===================================================
std::array<std::complex<float>, TABLE_SIZE> toComplexFFTArray(
    float* data,
    bool useImagPart) {
  std::array<std::complex<float>, TABLE_SIZE> comp;
  if (useImagPart) {
    for (size_t i = 0; i < TABLE_SIZE; ++i) {
      // jassert(std::fabs(data[i]) <= 1.0f);
      comp[i].imag(data[i]);
      comp[i].real(0.0f);
    }
  } else {
    for (size_t i = 0; i < TABLE_SIZE; ++i) {
      comp[i].real(data[i]);
      comp[i].imag(0.0f);
    }
  }
  return comp;
}

void wavetableFFTComplex(std::complex<float>* buf) {
  int N = TABLE_SIZE;
  int i, j, k, L;           /* indexes */
  int M, TEMP, LE, LE1, ip; /* M = log N */
  int NV2, NM1;
  double t; /* temp */
  float Ur, Ui, Wr, Wi, Tr, Ti;
  float Ur_old;
  // if ((N > 1) && !(N & (N - 1)))   // make sure we have a power of 2
  NV2 = N >> 1;
  NM1 = N - 1;
  TEMP = N; /* get M = log N */
  M = 0;
  while (TEMP >>= 1)
    ++M;
  /* shuffle */
  j = 1;
  for (i = 1; i <= NM1; i++) {
    if (i < j) { /* swap a[i] and a[j] */
      auto previousI = buf[i - 1];
      buf[i - 1] = buf[j - 1];
      buf[j - 1] = previousI;
    }
    k = NV2; /* bit-reversed counter */
    while (k < j) {
      j -= k;
      k /= 2;
    }
    j += k;
  }
  LE = 1.0f;
  for (L = 1; L <= M; L++) {  // stage L
    LE1 = LE;                 // (LE1 = LE/2)
    LE *= 2;                  // (LE = 2^L)
    Ur = 1.0f;
    Ui = 0.0f;
    Wr = std::cosf((float)M_PI / (float)LE1);
    Wi = -std::sinf((float)M_PI /
                    (float)LE1);  // Cooley, Lewis, and Welch have "+" here
    for (j = 1; j <= LE1; j++) {
      for (i = j; i <= N; i += LE) {  // butterfly
        ip = i + LE1;
        Tr = buf[ip - 1].real() * Ur - buf[ip - 1].imag() * Ui;
        Ti = buf[ip - 1].real() * Ui + buf[ip - 1].imag() * Ur;
        buf[ip - 1].real(buf[i - 1].real() - Tr);
        buf[ip - 1].imag(buf[i - 1].imag() - Ti);
        buf[i - 1].real(buf[i - 1].real() + Tr);
        buf[i - 1].imag(buf[i - 1].imag() + Ti);
      }
      Ur_old = Ur;
      Ur = Ur_old * Wr - Ui * Wi;
      Ui = Ur_old * Wi + Ui * Wr;
    }
  }
}

}  // namespace AudioUtil

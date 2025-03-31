#pragma once

#include <complex>
#include "../Common.h"

// some defines for the tuning range of our
// oscillators. matches the 88-key range of a grand
// piano
#define MIDI_NOTE_MIN 21
#define MIDI_NOTE_MAX 108

// # of samples in our wavetables
#define TABLE_SIZE 2048
/* Namespace for stuff that might
 * be useful in any number of situations
 * */
namespace AudioUtil {
/* Convert note/tuning
 * information into a
 * frequency expressed in
 * terms of phase change/sample
 * */
float phaseDeltForNote(int midiNote, float coarseSemis, float fineCents);
// call this on sample rate changes to keep
// the big brain tuning LUT thing working
void updateTuningTables(double sampleRate);
int fastFloor32(float fValue);
size_t fastFloor64(float fValue);
float fastSine(float phaseNorm);
inline float signed_flerp(float min, float max, float current, float t) {
  if (t > 0.0f)
    return flerp(current, max, t);
  return flerp(min, current, 1.0f - std::fabs(t));
}

// helpers for FFT stuff
std::array<std::complex<float>, TABLE_SIZE> toComplexFFTArray(
    float* data,
    bool useImagPart = true);

void wavetableFFTComplex(std::complex<float>* buf);
void wavetableFFTSplit(float* real, float* imag);
}  // namespace AudioUtil

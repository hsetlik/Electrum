#include "Electrum/Audio/HarmonicSeries.h"
// string encoding/decoding helpers
// static harmonic_t harmonicFromString(const String& str) {
//   float* pRatio = reinterpret_cast<float*>(str[0]);
//   float* pOffset = reinterpret_cast<float*>(str[1]);
//   float* pAmplitude = reinterpret_cast<float*>(str[2]);
//   return {*pRatio, *pOffset, *pAmplitude};
// }
//
// static String harmonicToString(harmonic_t h) {
//   String str = "";
//   str += *reinterpret_cast<wchar_t*>(&h.ratio);
//   str += *reinterpret_cast<wchar_t*>(&h.phaseOffset);
//   str += *reinterpret_cast<wchar_t*>(&h.amplitude);
//   return str;
// }
//===================================================

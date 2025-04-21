#pragma once

#include "Electrum/Identifiers.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include "juce_opengl/juce_opengl.h"

//==================================================================
typedef juce::Vector3D<float> vec3D_f;
typedef juce::Point<float> fpoint_t;
typedef juce::TabbedButtonBar TabBar;
typedef juce::AttributedString AttString;

struct BoundedAttString {
  AttString aString;
  frect_t bounds;
  BoundedAttString() = default;
  void draw(juce::Graphics& g) { aString.draw(g, bounds); }
};

namespace B64 {
inline float toFloat(const String& str) {
  float value = 0.0f;
  juce::MemoryOutputStream oStream(&value, sizeof(float));
  jassert(juce::Base64::convertFromBase64(oStream, str));
  return value;
}
inline String toString(float value) {
  return juce::Base64::toBase64(&value, sizeof(float));
}

inline size_t toSizeT(const String& str) {
  size_t value = 0;
  juce::MemoryOutputStream oStream(&value, sizeof(size_t));
  if (juce::Base64::convertFromBase64(oStream, str)) {
    return value;
  }
  return 0;
}

inline String toString(size_t value) {
  size_t temp = value;
  return juce::Base64::toBase64(&temp, sizeof(size_t));
}
}  // namespace B64

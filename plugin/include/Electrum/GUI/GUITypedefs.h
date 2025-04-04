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

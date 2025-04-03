#pragma once
#include <FontData.h>
#include "juce_graphics/juce_graphics.h"
typedef juce::Typeface::Ptr typeface_ptr;
// each of the fonts we've bundled in the binary
enum FontE {
  AcierDN,
  FuturaReg,
  FuturaBO,
  FuturaMO,
  FuturaLC,
  HelveticaMed,
  HelveticaReg,
  RobotoMI,
  RobotoTI,
  RobotoThin,
  HighwayGothhicW
};

#define NUM_BINARY_FONTS 11
namespace FontData {

juce::String getFontName(int idx);
juce::Font getFontWithHeight(FontE id, float height);

}  // namespace FontData

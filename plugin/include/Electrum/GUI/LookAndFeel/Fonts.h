#pragma once
#include <FontData.h>
#include "juce_graphics/juce_graphics.h"
typedef juce::Typeface::Ptr typeface_ptr;
// each of the fonts we've bundled in the binary
enum FontE {
  AcierDN,
  FuturaReg,
  FuturaBO,
  FuturaLC,
  FuturaMO,
  HelveticaMed,
  HelveticaReg,
  RobotoMI
};

#define NUM_BINARY_FONTS 8
namespace FontData {
juce::Font getFontWithHeight(FontE id, float height);

}  // namespace FontData

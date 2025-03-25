#pragma once
#include <FontData.h>
#include "juce_graphics/juce_graphics.h"
typedef juce::Typeface::Ptr typeface_ptr;
// each of the fonts we've bundled in the binary
enum FontE { AcierDN, FuturaReg, FuturaBO, FuturaLC, RobotoMI };

#define NUM_BINARY_FONTS 5
namespace FontData {

juce::String getFontName(int idx);
juce::Font getFontWithHeight(FontE id, float height);

}  // namespace FontData

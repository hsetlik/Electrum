#pragma once
/* This namespace is just a way of putting all our
 * color-related definitions and functions in a
 * single place
 * */

// had to do it sorry
#include "juce_graphics/juce_graphics.h"
#include "../../Shared/ElectrumState.h"
typedef juce::Colour color_t;

namespace Color {

//
#define DECLARE_COLOR(name, r, g, b) \
  const color_t name = color_t::fromRGB(r, g, b);
DECLARE_COLOR(closeRedBright, 209, 67, 67)
DECLARE_COLOR(closeRedDark, 147, 49, 49)
DECLARE_COLOR(black, 0, 0, 0)

DECLARE_COLOR(sandyYellow, 230, 197, 148)
DECLARE_COLOR(aquamarine, 127, 255, 212)
DECLARE_COLOR(salmon, 250, 128, 114)
DECLARE_COLOR(gainsboro, 220, 220, 220)
DECLARE_COLOR(darkSeaGreen, 143, 188, 143)

#undef DECLARE_COLOR

color_t getModSourceColor(ModSourceE src);
}  // namespace Color


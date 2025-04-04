#pragma once
/* This namespace is just a way of putting all our
 * color-related definitions and functions in a
 * single place
 * */

#include "juce_graphics/juce_graphics.h"
#include "../../Shared/ElectrumState.h"

// had to do it sorry
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

// these colors are taken from my nvim theme
DECLARE_COLOR(nearBlack, 0, 23, 41)
DECLARE_COLOR(darkBlue, 15, 41, 53)
DECLARE_COLOR(highlightedDarkBlue, 51, 100, 148)
DECLARE_COLOR(periwinkle, 115, 167, 250)
DECLARE_COLOR(literalOrangePale, 229, 183, 127)
DECLARE_COLOR(literalOrangeBright, 255, 134, 100)
DECLARE_COLOR(mintGreenPale, 98, 241, 217)
DECLARE_COLOR(mintGreenBright, 0, 229, 187)
DECLARE_COLOR(assignmentPink, 255, 68, 109)
DECLARE_COLOR(nameOffWhite, 217, 227, 244)
DECLARE_COLOR(commentGray, 132, 156, 154)
DECLARE_COLOR(qualifierPurple, 234, 157, 255)
DECLARE_COLOR(marginGray, 44, 48, 67)
DECLARE_COLOR(lineNumGray, 76, 100, 121)

#undef DECLARE_COLOR

// returns the
color_t getModSourceColor(ModSourceE src);
}  // namespace Color

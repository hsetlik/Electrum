#pragma once
/* This namespace is just a way of putting all our
 * color-related definitions and functions in a
 * single place
 * */

#include "juce_graphics/juce_graphics.h"
#include "../../Shared/ElectrumState.h"

#define DECLARE_COLOR(name, r, g, b) \
  const color_t name = color_t::fromRGB(r, g, b);
// had to do it sorry
typedef juce::Colour color_t;

namespace Color {

//

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

// returns the
color_t getModSourceColor(ModSourceE src);
}  // namespace Color

#undef DECLARE_COLOR
// easier/clearer access to the UI colors using this namespace
namespace UIColor {
const color_t windowBkgnd = Color::nearBlack;
const color_t menuBkgnd = Color::darkBlue;
const color_t widgetBkgnd = Color::marginGray;
const color_t outline = Color::darkBlue.brighter();
const color_t defaultText = Color::literalOrangePale;
const color_t menuText = Color::literalOrangePale;
const color_t defaultFill = Color::periwinkle;
const color_t highlightedText = Color::literalOrangeBright;
const color_t highlightedFill = Color::periwinkle;

}  // namespace UIColor

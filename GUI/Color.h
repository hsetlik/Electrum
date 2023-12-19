
#pragma once
#include "../Parameters/Identifiers.h"
#include "Utility/GUIUtil.h"
#ifndef COLOR_H
#define COLOR_H
namespace Color {
// a little macro to make globally defining and accessing colors easier
#define DECLARE_COLOR(name, r, g, b) const Colour name = Colour::fromRGB(r, g, b);

DECLARE_COLOR(closeRedBright, 209, 67, 67)
DECLARE_COLOR(closeRedDark, 147, 49, 49)
DECLARE_COLOR(black, 0, 0, 0)
DECLARE_COLOR(chartreuse, 127, 255, 0)
DECLARE_COLOR(mediumSeaGreen, 60, 179, 113)
DECLARE_COLOR(indigo, 75, 0, 130)
DECLARE_COLOR(lightSteelBlue, 176, 196, 222)
DECLARE_COLOR(aquamarine, 127, 255, 212)
DECLARE_COLOR(salmon, 250, 128, 114)
DECLARE_COLOR(gainsboro, 220, 220, 220)
DECLARE_COLOR(darkSeaGreen, 143, 188, 143)
DECLARE_COLOR(maroon, 128, 0, 0)

DECLARE_COLOR(offWhite, 196, 204, 218)
DECLARE_COLOR(darkBkgnd, 5, 22, 38)
DECLARE_COLOR(paleSeafoam, 147, 215, 201)
DECLARE_COLOR(paleOrange, 233, 145, 115)
DECLARE_COLOR(brightSeafoam, 94, 196, 169)
DECLARE_COLOR(deepPink, 236, 100, 118)
DECLARE_COLOR(sandyYellow, 230, 197, 148)
DECLARE_COLOR(mediumGray, 127, 142, 143)
DECLARE_COLOR(lavendar, 191, 150, 228)

#undef DECLARE_COLOR

// assign and access the color assigned to eact modulation source
const std::unordered_map<String, Colour> modSourceColors = {
    {IDs::modWheelSource.toString(), mediumSeaGreen},
    {IDs::pitchWheelSource.toString(), indigo},
    {IDs::perlinSource.toString(), chartreuse},
    {IDs::envSource.toString() + "0", salmon},
    {IDs::envSource.toString() + "1", lightSteelBlue},
    {IDs::envSource.toString() + "2", darkSeaGreen},
};

inline Colour getColorForModSource(const String &srcID)
{
  auto it = modSourceColors.find(srcID);
  if (it != modSourceColors.end())
    return it->second;
  return mediumSeaGreen;
}

} // namespace Color
#endif

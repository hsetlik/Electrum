
#pragma once
#include "../Parameters/Identifiers.h"
#include "Utility/GUIUtil.h"
#ifndef COLOR_H
#define COLOR_H
namespace Color
{
// a little macro to make globally defining and accessing colors easier
#define DECLARE_COLOR(name, r, g, b) const Colour name = Colour::fromRGB(r, g, b);
DECLARE_COLOR(darkSlateGray, 47, 79, 79)
DECLARE_COLOR(closeRedBright, 209, 67, 67)
DECLARE_COLOR(closeRedDark, 147, 49, 49)
DECLARE_COLOR(black, 0, 0, 0)
DECLARE_COLOR(brightYellow, 255, 236, 64)
DECLARE_COLOR(slateBlue, 106, 90, 205)
DECLARE_COLOR(chartreuse, 127, 255, 0)
DECLARE_COLOR(mediumSeaGreen, 60, 179, 113)
DECLARE_COLOR(indigo, 75, 0, 130)
DECLARE_COLOR(magenta, 255, 0, 255)
DECLARE_COLOR(lightSteelBlue, 176, 196, 222)
DECLARE_COLOR(purple, 128, 0, 128)
DECLARE_COLOR(aquamarine, 127, 255, 212)
DECLARE_COLOR(oliveDrab, 107, 102, 35)
DECLARE_COLOR(salmon, 250, 128, 114)
DECLARE_COLOR(gainsboro, 220, 220, 220)
#undef DECLARE_COLOR

// assign and access the color assigned to eact modulation source
const std::unordered_map<String, Colour> modSourceColors = 
{
    { IDs::modWheelSource.toString(), mediumSeaGreen},
    { IDs::pitchWheelSource.toString(), indigo},
    { IDs::perlinSource.toString(), chartreuse},
};

inline Colour getColorForModSource(const String& srcID)
{
    auto it = modSourceColors.find(srcID);
    if (it != modSourceColors.end())
        return it->second;
    return magenta;
}

}
#endif
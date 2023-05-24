
#pragma once
#include "../Parameters/Identifiers.h"
#include "GUIUtil.h"
#ifndef COLOR_H
#define COLOR_H
namespace Color
{
// a little macro to make globally defining and accessing colors easier
#define DECLARE_COLOR(name, r, g, b) const Colour name = Colour::fromRGB(r, g, b);
DECLARE_COLOR(lightGray, 142, 145, 145)
DECLARE_COLOR(lightTeal, 94, 224, 222)
DECLARE_COLOR(darkGray, 71, 71, 71)
DECLARE_COLOR(closeRedBright, 209, 67, 67)
DECLARE_COLOR(closeRedDark, 147, 49, 49)
#undef DECLARE_COLOR
}
#endif
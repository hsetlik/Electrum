
#pragma once
#include "../Parameters/Identifiers.h"
#ifndef COLOR_H
#define COLOR_H
namespace Color
{
#define DECLARE_COLOR(name, r, g, b) const Colour name(#r, #g, #b);

#undef DECLARE_COLOR(lightGray, 142, 145, 145);
}
#endif
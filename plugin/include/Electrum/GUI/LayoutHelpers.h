#pragma once
#include "../Identifiers.h"
#include "juce_graphics/juce_graphics.h"

namespace Layout {
typedef std::vector<std::vector<frect_t>> fgrid_t;
// divdes a rectangle into an evenly sized grid with the
// given number of rows and columns
fgrid_t divideInto(frect_t limits, int numColumns, int numRows);
// returns the input rectangle with space left above
// to fit the given TextLayout
frect_t leaveRoomAbove(const frect_t& limits, juce::TextLayout& text);
// trims the rectangle's longer side to match the shorter one.
// optionally maintains the rect's previous center point
frect_t makeSquare(const frect_t& rect,
                   bool maintainXCenter = false,
                   bool maintainYCenter = false);
}  // namespace Layout
// various useful math/geometry helpers

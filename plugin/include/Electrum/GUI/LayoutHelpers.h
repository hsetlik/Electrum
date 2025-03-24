#pragma once
#include "../Identifiers.h"

namespace Layout {
typedef std::vector<std::vector<frect_t>> fgrid_t;
fgrid_t divideInto(frect_t limits, int numColumns, int numRows);
}  // namespace Layout
// various useful math/geometry helpers


#include "Electrum/GUI/LayoutHelpers.h"

namespace Layout {
fgrid_t divideInto(frect_t limits, int numColumns, int numRows) {
  fgrid_t grid = {};
  const float x0 = limits.getX();
  const float y0 = limits.getY();
  const float dX = limits.getWidth() / (float)numColumns;
  const float dY = limits.getHeight() / (float)numRows;
  float x, y;
  for (int c = 0; c < numColumns; ++c) {
    std::vector<frect_t> col = {};
    for (int r = 0; r < numRows; ++r) {
      x = x0 + (dX * (float)c);
      y = y0 + (dY * (float)r);
      col.push_back({x, y, dX, dY});
    }
    grid.push_back(col);
  }
  return grid;
}
}  // namespace Layout
//===================================================

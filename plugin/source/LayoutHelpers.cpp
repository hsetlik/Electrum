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

frect_t leaveRoomAbove(const frect_t& limits, juce::TextLayout& text) {
  jassert(text.getHeight() < limits.getHeight());
  const float dY = text.getHeight();
  return {limits.getX(), limits.getY() + dY, limits.getWidth(),
          limits.getHeight() - dY};
}

frect_t makeSquare(const frect_t& rect, bool keepXCenter, bool keepYCenter) {
  float x = rect.getX();
  float y = rect.getY();
  const float sideLength = std::min(rect.getHeight(), rect.getWidth());
  if (keepXCenter) {
    const float xCenter = x + (rect.getWidth() / 2.0f);
    x = xCenter - (sideLength / 2.0f);
  }
  if (keepYCenter) {
    const float yCenter = y + (rect.getHeight() / 2.0f);
    y = yCenter - (sideLength / 2.0f);
  }
  return {x, y, sideLength, sideLength};
}

}  // namespace Layout
//===================================================

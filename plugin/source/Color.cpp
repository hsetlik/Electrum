#include "Electrum/GUI/LookAndFeel/Color.h"
#include "Electrum/Shared/ElectrumState.h"

namespace Color {

static color_t modSrcColors[MOD_SOURCES] = {sandyYellow, aquamarine, salmon,
                                            gainsboro, darkSeaGreen};
color_t getModSourceColor(ModSourceE src) {
  return modSrcColors[(size_t)src];
}
}  // namespace Color
//===================================================

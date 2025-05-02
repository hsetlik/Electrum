#include "Electrum/GUI/LookAndFeel/Color.h"
#include "Electrum/Shared/ElectrumState.h"

namespace Color {

static color_t modSrcColors[MOD_SOURCES] = {
    sandyYellow,       aquamarine,      salmon,
    gainsboro,         darkSeaGreen,    highlightedDarkBlue,
    literalOrangePale, assignmentPink,  qualifierPurple.darker(0.2f),
    periwinkle,        mintGreenBright, assignmentPink.darker(0.2f)};
color_t getModSourceColor(ModSourceE src) {
  return modSrcColors[(size_t)src];
}
}  // namespace Color
//===================================================

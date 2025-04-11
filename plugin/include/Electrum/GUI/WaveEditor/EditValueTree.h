#pragma once
#include <juce_core/juce_core.h>
#include "Electrum/Identifiers.h"

#define MAX_BIN 1024
/* Namespace for shared Identifiers and such
 * */
namespace WaveEdit {
#define DECLARE_ID(name) const juce::Identifier name(#name);
DECLARE_ID(WAVETABLE)
DECLARE_ID(waveName)
// represents one wave in the table
DECLARE_ID(WAVE_FRAME)
DECLARE_ID(frameIndex)
DECLARE_ID(frameStringData)

// each WAVE_FRAME tree can have a number of children to represent
// various edits
DECLARE_ID(BIN_GAIN_PT)
//
DECLARE_ID(binIdx)
DECLARE_ID(binGainDb)

DECLARE_ID(RAND_PHASES)

#undef DECLARE_ID
// get the raw WAVETABLE tree for the wave file with a given name
ValueTree getWavetableTree(const String& name);
}  // namespace WaveEdit

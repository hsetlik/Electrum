#pragma once
#include <juce_core/juce_core.h>
#include "Electrum/Identifiers.h"
#include "Electrum/Shared/FileSystem.h"

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

// parse the top-level WAVETABLE tree into a metadata object for
// saving and validating

// just add up all the strings for the full Wavetable
String getFullWavetableString(const ValueTree& tree);

// these functions do the work of applying the various fixed transforms to the
// wave
// void randomizeFramePhases(ValueTree& waveTree, int idx);
}  // namespace WaveEdit

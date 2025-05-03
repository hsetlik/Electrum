#pragma once
#include "juce_gui_basics/juce_gui_basics.h"

typedef juce::ApplicationCommandTarget CommandTarget;
typedef juce::ApplicationCommandInfo CommandInfo;
typedef juce::CommandID CommandID;

namespace WaveCmd {
// Basic frame adding/removing--------------
const CommandID appendWaveFrame = 0;
const CommandID insertWaveFrame = 1;
const CommandID deleteWaveFrame = 2;
// Interpolation--------------------------
const CommandID insertLerpFrames = 3;
// File menu stuff-----------------------
const CommandID newWavetable = 4;
const CommandID openWavetable = 5;

static constexpr size_t NUM_WAVE_COMMANDS = 6;

// get the info for each of the commands
CommandInfo getInfo(CommandID id);

}  // namespace WaveCmd

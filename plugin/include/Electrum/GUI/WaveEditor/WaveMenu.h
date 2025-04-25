#pragma once
#include "juce_gui_basics/juce_gui_basics.h"

typedef juce::ApplicationCommandTarget CommandTarget;
typedef juce::CommandID CommandID;

namespace WaveCmd {
// Basic frame adding/removing--------------
const CommandID appendWaveFrame = 0;
const CommandID insertWaveFrame = 1;
const CommandID deleteWaveFrame = 2;
// wave selecting--------------------------
const CommandID focusWaveFrame = 3;

}  // namespace WaveCmd

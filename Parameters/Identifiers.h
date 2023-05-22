#ifndef IDENTIFIERS_H
#define IDENTIFIERS_H
#include "../Core/CustomJuceHeader.h"
#include "DLog.h"
// Every Identifier for every parameter should be in here
#define NUM_OSCILLATORS 3
#define OSC_POS_DEFAULT 0.1f
#define OSC_LEVEL_DEFAULT 0.8f

using frange = NormalisableRange<float>;
namespace IDs
{
#define DECLARE_ID(name) const juce::Identifier name (#name);
DECLARE_ID(ELECTRUM_STATE)
// oscillator
DECLARE_ID(oscillatorPos)
DECLARE_ID(oscillatorLevel)
// envelope
DECLARE_ID(attackMs)
DECLARE_ID(decayMs)

DECLARE_ID(WAVETABLE_DATA)
DECLARE_ID(wavetableName)
DECLARE_ID(wavetableSize)
DECLARE_ID(wavetableStringData)

DECLARE_ID(ELECTRUM_MODULATIONS) // tree type for subtree that contains modulation data
DECLARE_ID(MODULATION)
DECLARE_ID(modulationSource)
DECLARE_ID(modulationDest)
DECLARE_ID(modulationDepth)

// modulation sources
DECLARE_ID(modWheelSource)



#undef DECLARE_ID

inline AudioProcessorValueTreeState::ParameterLayout createElectrumLayout()
{
    AudioProcessorValueTreeState::ParameterLayout layout;
    frange posRange(0.0f, 1.0f, 0.0001f);
    frange levelRange(0.0f, 1.0f, 0.0001f);
    for (int i = 0; i < NUM_OSCILLATORS; i++)
    {
        auto iStr = juce::String(i);
        String positionId = oscillatorPos.toString() + iStr;
        String levelId = oscillatorLevel.toString() + iStr;
        auto levelName = "Oscillator " + iStr + " level";
        auto positionName = "Oscillator " + iStr + " position";
        DLog::log("Osc Level: " + levelId);
        DLog::log("Osc Pos: " + positionId);
        layout.add(std::make_unique<AudioParameterFloat>(positionId, positionId, posRange, OSC_POS_DEFAULT));
        layout.add(std::make_unique<AudioParameterFloat>(levelId, levelId, levelRange, OSC_LEVEL_DEFAULT));
    }
    return layout;
}

}

#endif
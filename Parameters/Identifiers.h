#ifndef IDENTIFIERS_H
#define IDENTIFIERS_H
#include "../Core/CustomJuceHeader.h"
#include "DLog.h"
// Every Identifier for every parameter should be in here
#define NUM_OSCILLATORS 3
#define OSC_POS_DEFAULT 0.1f
#define OSC_LEVEL_DEFAULT 0.8f

#define PERLIN_OCTAVES_DEFAULT 2
#define PERLIN_FREQ_DEFAULT 3.0f
#define PERLIN_LAC_DEAULT 2.0f

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


//perlin noise generaion
DECLARE_ID(perlinFreq)
DECLARE_ID(perlinOctaves)
DECLARE_ID(perlinLacunarity)

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
DECLARE_ID(pitchWheelSource)
DECLARE_ID(perlinSource)

const std::vector<Identifier> ElectrumIDs = 
{
    oscillatorPos,
    oscillatorLevel,

    attackMs,
    decayMs,

    wavetableName,
    wavetableSize,
    wavetableStringData,

    modulationSource,
    modulationDest,
    modulationDepth,

    perlinFreq,
    perlinOctaves,
    perlinLacunarity,

    modWheelSource,
    pitchWheelSource,
    perlinSource
};

#undef DECLARE_ID


inline AudioProcessorValueTreeState::ParameterLayout createElectrumLayout()
{
    AudioProcessorValueTreeState::ParameterLayout layout;
    frange posRange(0.0f, 1.0f, 0.0001f);
    frange levelRange(0.0f, 1.0f, 0.0001f);
    // oscillator params
    for (int i = 0; i < NUM_OSCILLATORS; i++)
    {
        auto iStr = juce::String(i);
        String positionId = oscillatorPos.toString() + iStr;
        String levelId = oscillatorLevel.toString() + iStr;
        auto levelName = "Oscillator " + iStr + " level";
        auto positionName = "Oscillator " + iStr + " position";
        layout.add(std::make_unique<AudioParameterFloat>(positionId, positionName, posRange, OSC_POS_DEFAULT));
        layout.add(std::make_unique<AudioParameterFloat>(levelId, levelName, levelRange, OSC_LEVEL_DEFAULT));
    }
    // perlin noise params
    frange pFreqRange(0.1f, 100.0f, 0.0001f);
    String pFreqID = perlinFreq.toString();
    String pFreqName = "Perlin Frequency";
    layout.add(std::make_unique<AudioParameterFloat>(pFreqID, pFreqName, pFreqRange, PERLIN_FREQ_DEFAULT));

    String pOctID = perlinOctaves.toString();
    String pOctName = "Perlin Noise Octaves";
    layout.add(std::make_unique<AudioParameterInt>(pOctID, pOctName, 1, 20, PERLIN_OCTAVES_DEFAULT));

    frange lacunarityRange(1.5f, 15.0f, 0.0001f);
    String pLacID = perlinLacunarity.toString();
    String pLacName = "Perlin Noise Lacunarity";
    layout.add(std::make_unique<AudioParameterFloat>(pLacID, pLacName, lacunarityRange, PERLIN_LAC_DEAULT));

    return layout;
}

}

#endif
#ifndef IDENTIFIERS_H
#define IDENTIFIERS_H
#include "../Core/CustomJuceHeader.h"
#include "StringUtil.h"
#include "DLog.h"
// Every Identifier for every parameter should be in here
#define NUM_OSCILLATORS 3
#define NUM_ENVELOPES 3
#define OSC_POS_DEFAULT 0.1f
#define OSC_LEVEL_DEFAULT 0.8f

#define ATK_MS_DEFAULT 20.0f
#define HOLD_MS_DEFAULT 0.0f
#define DECAY_MS_DEFAULT 50.0f
#define SUSTAIN_LEVEL_DEFAULT 0.65f
#define RELEASE_MS_DEFAULT 85.0f

#define CONTROL_LENGTH_DEFAULT 0.5f
#define CONTROL_ANGLE_DEFAULT MathConstants<float>::pi / 4.0f


#define PERLIN_OCTAVES_DEFAULT 2
#define PERLIN_FREQ_DEFAULT 1.0f
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
DECLARE_ID(attackC1Length) // distance from the origin of the first control point
DECLARE_ID(attackC1Angle) // control point 1's angle in radians clockwise from 12 o'clock
DECLARE_ID(attackC2Length) 
DECLARE_ID(attackC2Angle)

DECLARE_ID(holdMs)

DECLARE_ID(decayMs)
DECLARE_ID(decayC1Length) 
DECLARE_ID(decayC1Angle)
DECLARE_ID(decayC2Length) 
DECLARE_ID(decayC2Angle)

DECLARE_ID(sustainLevel)

DECLARE_ID(releaseMs)
DECLARE_ID(releaseC1Length) 
DECLARE_ID(releaseC1Angle)
DECLARE_ID(releaseC2Length) 
DECLARE_ID(releaseC2Angle)



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
DECLARE_ID(envSource)

const std::vector<Identifier> ElectrumIDs = 
{
    oscillatorPos,
    oscillatorLevel,

    attackMs,
    attackC1Length,
    attackC1Angle,
    attackC2Length,
    attackC2Angle,

    holdMs,

    decayMs,
    decayC1Length,
    decayC1Angle,
    decayC2Length,
    decayC2Angle,

    sustainLevel,

    releaseMs,
    releaseC1Length,
    releaseC1Angle,
    releaseC2Length,
    releaseC2Angle,

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
    perlinSource,
    envSource
};

#undef DECLARE_ID

struct ParamInfoStrings
{
    String shortName;
    String longName;
    String desc;
};

const std::unordered_map<String, ParamInfoStrings> paramDisplayNames = 
{
    // oscillators
    {oscillatorPos.toString(), {"Osc. pos.", "Wavetable Oscillator Position", "Current position (range 0-1) in this oscillator's set of wavetables"}},
    {oscillatorLevel.toString(), {"Osc. level", "Oscilator level", "The oscillator's output level"}},
    // perlin
    {perlinFreq.toString(), {"Freq.", "Perlin Noise Frequency", "How quickly the Perlin noise engine advances"}},
    {perlinOctaves.toString(), {"Octaves", "Perlin Noise Octaves", "The number of layers used by the Perlin noise algorithm"}},
    {perlinLacunarity.toString(), {"Lac.", "Perlin Noise Lacunarity", "The relative increase in frequency for each octave of Perlin noise"}},
    // envelopes
    {attackMs.toString(), {"Atk.", "Envelope attack time", "Attack: number of miliseconds between a note beginning and the envelope reaching its full lever"}},
    {attackC1Length.toString(), {"AtkL1", "Attack control spline 1 length", "Controls level shape curve for the envelope's attack"}},
    {attackC1Angle.toString(), {"AtkA1", "Attack control spline 1 angle", "Controls level shape curve for the envelope's attack"}},
    {attackC2Length.toString(), {"AtkL2", "Attack control spline 2 length", "Controls level shape curve for the envelope's attack"}},
    {attackC2Angle.toString(), {"AtkA2", "Attack control spline 2 angle", "Controls level shape curve for the envelope's attack"}},

    {holdMs.toString(), {"Hold", "Peak hold time", "Hold: the number of miliseconds for which the envelope with rest at max lever after the attack phase is over"}},

    {decayMs.toString(), {"Decay", "Envelope decay time", "Decay: time from the envelope hitting its peak and reaching its sustain level"}},
    {decayC1Length.toString(), {"DckL1", "Decay control spline 1 length", "Controls level shape curve for the envelope's decay"}},
    {decayC1Angle.toString(), {"DckA1", "Decay control spline 1 angle", "Controls level shape curve for the envelope's decay"}},
    {decayC2Length.toString(), {"DckL2", "Decay control spline 2 length", "Controls level shape curve for the envelope's decay"}},
    {decayC2Angle.toString(), {"DckA2", "Decay control spline 2 angle", "Controls level shape curve for the envelope's decay"}},

    {sustainLevel.toString(), {"Sustain", "Envelope sustain level", "Sustain: the level which the envelope maintains after the decay stage has finished and before the envelope's gate has ended"}},

    {releaseMs.toString(), {"Release", "Envelope release time", "Release: the time from the gate ending to the envelope output reaching 0"}},
    {releaseC1Length.toString(), {"RlsL1", "Release control spline 1 length", "Controls level shape curve for the envelope's release"}},
    {releaseC1Angle.toString(), {"RlsA1", "Release control spline 1 angle", "Controls level shape curve for the envelope's release"}},
    {releaseC2Length.toString(), {"RlsL2", "Release control spline 2 length", "Controls level shape curve for the envelope's release"}},
    {releaseC2Angle.toString(), {"RlsA2", "Release control spline 2 angle", "Controls level shape curve for the envelope's release"}}
};

inline String getParamName(const String& paramID, bool longName=false)
{   
    String safeParamID = StringUtil::removeTrailingNumbers(paramID);
    auto it = paramDisplayNames.find(safeParamID);
    if(it != paramDisplayNames.end())
    {
        return longName ? it->second.longName : it->second.shortName;
    }
    DLog::log("Warning! No name info found for parameter: " + safeParamID);
    return "";
}

inline AudioProcessorValueTreeState::ParameterLayout createElectrumLayout()
{
    AudioProcessorValueTreeState::ParameterLayout layout;
    frange posRange(0.0f, 1.0f, 0.0001f);
    frange levelRange(0.0f, 1.0f, 0.0001f);
    // oscillator params
    for (int i = 0; i < NUM_OSCILLATORS; i++)
    {
        auto iStr = String(i);
        String positionId = oscillatorPos.toString() + iStr;
        String levelId = oscillatorLevel.toString() + iStr;
        auto levelName = getParamName(positionId, true);
        auto positionName = getParamName(levelId, true);
        layout.add(std::make_unique<AudioParameterFloat>(positionId, positionName, posRange, OSC_POS_DEFAULT));
        layout.add(std::make_unique<AudioParameterFloat>(levelId, levelName, levelRange, OSC_LEVEL_DEFAULT));
    }
    //envelopes
    frange atkRange(0.1f, 2500.0f, 0.0001f);
    frange holdRange(0.0f, 100.0f, 0.0001f);
    frange decayRange(0.1f, 4000.0f, 0.0001f);
    frange releaseRange(1.0f, 10000.0f, 0.0001f);
    frange angleRange(0.0f, MathConstants<float>::pi / 2.0f, 0.0001f);
    frange cLengthRange(0.0f, 1.0f, 0.00001f);
    for(int i = 0; i < NUM_ENVELOPES; i++)
    {
        // attack MS
        auto iStr = String(i);
        String aMsID = attackMs.toString() + iStr;
        layout.add(std::make_unique<AudioParameterFloat>(aMsID, getParamName(aMsID, true), atkRange, ATK_MS_DEFAULT));
        // attack control points
        String aC1LengthID = attackC1Length.toString() + iStr;
        layout.add(std::make_unique<AudioParameterFloat>(aC1LengthID, getParamName(aC1LengthID, true), cLengthRange, CONTROL_LENGTH_DEFAULT));
        String aC1AngleID = attackC1Angle.toString() + iStr;
        layout.add(std::make_unique<AudioParameterFloat>(aC1AngleID, getParamName(aC1AngleID, true), angleRange, CONTROL_ANGLE_DEFAULT));   
        String aC2LengthID = attackC2Length.toString() + iStr;
        layout.add(std::make_unique<AudioParameterFloat>(aC2LengthID, getParamName(aC2LengthID, true), cLengthRange, CONTROL_LENGTH_DEFAULT));
        String aC2AngleID = attackC2Angle.toString() + iStr;
        layout.add(std::make_unique<AudioParameterFloat>(aC2AngleID, getParamName(aC2AngleID, true), angleRange, CONTROL_ANGLE_DEFAULT));   
        //hold Ms
        String holdID = holdMs.toString() + iStr;
        layout.add(std::make_unique<AudioParameterFloat>(holdID, getParamName(holdID, true), holdRange, HOLD_MS_DEFAULT));
        // decay Ms
        String decayID = decayMs.toString() + iStr;
        layout.add(std::make_unique<AudioParameterFloat>(decayID, getParamName(decayID, true), decayRange, DECAY_MS_DEFAULT));
        // decay control points
        String decC1LengthID = decayC1Length.toString() + iStr;
        layout.add(std::make_unique<AudioParameterFloat>(decC1LengthID, getParamName(decC1LengthID, true), cLengthRange, CONTROL_LENGTH_DEFAULT));
        String decC1AngleID = decayC1Angle.toString() + iStr;
        layout.add(std::make_unique<AudioParameterFloat>(decC1AngleID, getParamName(decC1AngleID, true), angleRange, CONTROL_ANGLE_DEFAULT));   
        String decC2LengthID = decayC2Length.toString() + iStr;
        layout.add(std::make_unique<AudioParameterFloat>(decC2LengthID, getParamName(decC2LengthID, true), cLengthRange, CONTROL_LENGTH_DEFAULT));
        String decC2AngleID = decayC2Angle.toString() + iStr;
        layout.add(std::make_unique<AudioParameterFloat>(decC2AngleID, getParamName(decC2AngleID, true), angleRange, CONTROL_ANGLE_DEFAULT));
        //sustain level
        String sustainID = sustainLevel.toString() + iStr;
        layout.add(std::make_unique<AudioParameterFloat>(sustainID, getParamName(sustainID, true), 0.0f, 0.1f, SUSTAIN_LEVEL_DEFAULT));
        //release Ms
        String releaseID = releaseMs.toString() + iStr;
        layout.add(std::make_unique<AudioParameterFloat>(releaseID, getParamName(releaseID, true), releaseRange, RELEASE_MS_DEFAULT));
        //release control points
        String relC1LengthID = releaseC1Length.toString() + iStr;
        layout.add(std::make_unique<AudioParameterFloat>(relC1LengthID, getParamName(relC1LengthID, true), cLengthRange, CONTROL_LENGTH_DEFAULT));
        String relC1AngleID = releaseC1Angle.toString() + iStr;
        layout.add(std::make_unique<AudioParameterFloat>(relC1AngleID, getParamName(relC1AngleID, true), angleRange, CONTROL_ANGLE_DEFAULT));   
        String relC2LengthID = releaseC2Length.toString() + iStr;
        layout.add(std::make_unique<AudioParameterFloat>(relC2LengthID, getParamName(relC2LengthID, true), cLengthRange, CONTROL_LENGTH_DEFAULT));
        String relC2AngleID = releaseC2Angle.toString() + iStr;
        layout.add(std::make_unique<AudioParameterFloat>(relC2AngleID, getParamName(relC2AngleID, true), angleRange, CONTROL_ANGLE_DEFAULT));
    }
    // perlin noise params
    frange pFreqRange(0.25f, 30.0f, 0.0001f);
    pFreqRange.setSkewForCentre(5.0f);
    String pFreqID = perlinFreq.toString();
    String pFreqName = "Perlin Frequency";
    layout.add(std::make_unique<AudioParameterFloat>(pFreqID, pFreqName, pFreqRange, PERLIN_FREQ_DEFAULT));

    String pOctID = perlinOctaves.toString();
    String pOctName = "Perlin Noise Octaves";
    layout.add(std::make_unique<AudioParameterInt>(pOctID, pOctName, 1, 20, PERLIN_OCTAVES_DEFAULT));

    frange lacunarityRange(1.0f, 5.0f, 0.0001f);
    String pLacID = perlinLacunarity.toString();
    String pLacName = "Perlin Noise Lacunarity";
    layout.add(std::make_unique<AudioParameterFloat>(pLacID, pLacName, lacunarityRange, PERLIN_LAC_DEAULT));

    return layout;
}

}

#endif
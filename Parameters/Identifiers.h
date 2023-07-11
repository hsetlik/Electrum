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

#define ENV_CURVE_MIN 0.0f
#define ENV_CURVE_MAX 1.0f
#define ENV_CURVE_DEFAULT 0.5f

#define ATTACK_MS_DEFAULT 20.0f
#define ATTACK_MS_MIN 0.01f
#define ATTACK_MS_MAX 2500.0f
#define ATTACK_MS_CENTER 150.0f

#define HOLD_MS_DEFAULT 0.0f
#define HOLD_MS_MIN 0.0f
#define HOLD_MS_MAX 1000.0f
#define HOLD_MS_CENTER 100.0f

#define DECAY_MS_DEFAULT 50.0f
#define DECAY_MS_MIN 0.1f
#define DECAY_MS_MAX 4000.0f
#define DECAY_MS_CENTER 250.0f

#define SUSTAIN_LEVEL_DEFAULT 0.65f

#define RELEASE_MS_DEFAULT 85.0f
#define RELEASE_MS_MIN 1.0f
#define RELEASE_MS_MAX 10000.0f
#define RELEASE_MS_CENTER 400.0f

#define ENV_MS_MAX ATTACK_MS_MAX + HOLD_MS_MAX + DECAY_MS_MAX + RELEASE_MS_MAX

#define CUTOFF_HZ_MIN 20.0f
#define CUTOFF_HZ_MAX 20000.0f
#define CUTOFF_HZ_CENTER 1000.0f
#define CUTOFF_HZ_DEFAULT 1200.0f

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
DECLARE_ID(attackCurve)
DECLARE_ID(holdMs)
DECLARE_ID(decayMs)
DECLARE_ID(decayCurve)
DECLARE_ID(sustainLevel)
DECLARE_ID(releaseMs)
DECLARE_ID(releaseCurve)

// filter
DECLARE_ID(filterType)
DECLARE_ID(filterCutoff)
DECLARE_ID(filterResonance)
DECLARE_ID(filterMix)
DECLARE_ID(filterTracking)

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
    attackCurve,    
    holdMs,

    decayMs,
    decayCurve,
    sustainLevel,
    
    releaseMs,
    releaseCurve,

    filterType,
    filterCutoff,
    filterResonance,
    filterMix,
    filterTracking,

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

const StringArray filterTypes = {"Low Pass 12", "High Pass 12"};
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
    {attackCurve.toString(), {"AtkCrv", "Envelope attack curve", "Attack curve: this value corresponds to the level of the curve's midpoint"}},

    {holdMs.toString(), {"Hold", "Peak hold time", "Hold: the number of miliseconds for which the envelope with rest at max lever after the attack phase is over"}},

    {decayMs.toString(), {"Decay", "Envelope decay time", "Decay: time from the envelope hitting its peak and reaching its sustain level"}},
    {decayCurve.toString(), {"DcyCrv", "Envelope decay curve", "Decay curve: this value corresponds to the level of the curve's midpoint"}},

    {sustainLevel.toString(), {"Sustain", "Envelope sustain level", "Sustain: the level which the envelope maintains after the decay stage has finished and before the envelope's gate has ended"}},

    {releaseMs.toString(), {"Release", "Envelope release time", "Release: the time from the gate ending to the envelope output reaching 0"}},
    {releaseCurve.toString(), {"RlsCrv", "Envelope release curve", "Release curve: this value corresponds to the level of the curve's midpoint"}},
    // Filter
    {filterType.toString(), {"Type", "Filter type", "Filter type: the type of filter for the main synth voices"}},
    {filterCutoff.toString(), {"Cutoff", "Filter cutoff frequency", "Filter cutoff: the main filter's cutoff frequency"}},
    {filterResonance.toString(), {"Resonance", "Filter resonance", "Filter resonance: percentage of this filter type's max resonance to use"}},
    {filterMix.toString(), {"Mix", "Filter wet/dry mix", "Filter mix: the proportion of filtered signal to original signal for the output"}},
    {filterTracking.toString(), {"Tracking", "Filter key tracking", "Filter key tracking: how much the note's fundamental pitch should change the filter offset"}},

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

inline frange getAttackRange()
{
    frange range(ATTACK_MS_MIN, ATTACK_MS_MAX, 0.0001f);
    range.setSkewForCentre(ATTACK_MS_CENTER);
    return range;
}

inline frange getHoldRange()
{
    frange range(HOLD_MS_MIN, HOLD_MS_MAX, 0.0001f);
    range.setSkewForCentre(HOLD_MS_CENTER);
    return range;
}

inline frange getDecayRange()
{
    frange range(DECAY_MS_MIN, DECAY_MS_MAX, 0.0001f);
    range.setSkewForCentre(DECAY_MS_CENTER);
    return range;
}

inline frange getSustainRange()
{
    frange range(0.0f, 1.0f, 0.00000f);
    return range;
}

inline frange getReleaseRange()
{
    frange range(RELEASE_MS_MIN, RELEASE_MS_MAX, 0.0001f);
    range.setSkewForCentre(RELEASE_MS_CENTER);
    return range;
}

inline frange getCutoffRange()
{
  frange range(CUTOFF_HZ_MIN, CUTOFF_HZ_MAX, 0.00001f);
  range.setSkewForCentre(CUTOFF_HZ_CENTER);
  return range;
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
    frange curveRange(ENV_CURVE_MIN, ENV_CURVE_MAX, 0.0001f);
    auto atkRange = getAttackRange();
    auto holdRange = getHoldRange();
    auto decayRange = getDecayRange();
    auto releaseRange = getReleaseRange();
    for(int i = 0; i < NUM_ENVELOPES; i++)
    {
        // attack MS
        auto iStr = String(i);
        String aMsID = attackMs.toString() + iStr;
        layout.add(std::make_unique<AudioParameterFloat>(aMsID, getParamName(aMsID, true), atkRange, ATTACK_MS_DEFAULT));
        String aCurveID = attackCurve.toString() + iStr;
        layout.add(std::make_unique<AudioParameterFloat>(aCurveID, getParamName(aCurveID, true), curveRange, ENV_CURVE_DEFAULT));
        //hold Ms
        String holdID = holdMs.toString() + iStr;
        layout.add(std::make_unique<AudioParameterFloat>(holdID, getParamName(holdID, true), holdRange, HOLD_MS_DEFAULT));
        // decay Ms
        String decayID = decayMs.toString() + iStr;
        layout.add(std::make_unique<AudioParameterFloat>(decayID, getParamName(decayID, true), decayRange, DECAY_MS_DEFAULT));
        String dCurveID = decayCurve.toString() + iStr;
        layout.add(std::make_unique<AudioParameterFloat>(dCurveID, getParamName(dCurveID, true), curveRange, ENV_CURVE_DEFAULT));
        //sustain level
        String sustainID = sustainLevel.toString() + iStr;
        layout.add(std::make_unique<AudioParameterFloat>(sustainID, getParamName(sustainID, true), 0.0f, 1.0f, SUSTAIN_LEVEL_DEFAULT));
        //release Ms
        String releaseID = releaseMs.toString() + iStr;
        layout.add(std::make_unique<AudioParameterFloat>(releaseID, getParamName(releaseID, true), releaseRange, RELEASE_MS_DEFAULT));
        String rCurveID = releaseCurve.toString() + iStr;
        layout.add(std::make_unique<AudioParameterFloat>(rCurveID, getParamName(rCurveID, true), curveRange, ENV_CURVE_DEFAULT));

    }
    // filter params
    String fTypeID = filterType.toString();
    layout.add(std::make_unique<AudioParameterChoice>(fTypeID, getParamName(fTypeID, true), filterTypes, 0));
    String cutoffID = filterCutoff.toString();
    layout.add(std::make_unique<AudioParameterFloat>(cutoffID, getParamName(cutoffID, true), getCutoffRange(), CUTOFF_HZ_DEFAULT));
    String resID = filterResonance.toString();
    layout.add(std::make_unique<AudioParameterFloat>(resID, getParamName(resID, true), 0.0f, 1.0f, 0.0f));
    String filterMixID = filterMix.toString();
    layout.add(std::make_unique<AudioParameterFloat>(filterMixID, getParamName(filterMixID, true), 0.0f, 1.0f, 1.0f));
    String trackingID = filterTracking.toString();
    layout.add(std::make_unique<AudioParameterFloat>(trackingID, getParamName(trackingID, true), 0.0f, 1.0f, 1.0f));
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

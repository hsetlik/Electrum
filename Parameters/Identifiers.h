#ifndef IDENTIFIERS_H
#define IDENTIFIERS_H
#include "../Core/CustomJuceHeader.h"
#include "DLog.h"
#include "StringUtil.h"
// Every Identifier for every parameter should be in here
#define NUM_OSCILLATORS 3
#define NUM_ENVELOPES 3
#define NUM_LFOS 3

// oscillator
#define OSC_POS_DEFAULT 0.1f
#define OSC_LEVEL_DEFAULT 0.25f

#define COARSE_TUNE_MIN -36.0f
#define COARSE_TUNE_MAX 36.0f

#define FINE_TUNE_MIN -100.0f
#define FINE_TUNE_MAX 100.0f

// envelope
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
#define VEL_TRACKING_DEFAULT 0.85f

#define RELEASE_MS_DEFAULT 85.0f
#define RELEASE_MS_MIN 1.0f
#define RELEASE_MS_MAX 10000.0f
#define RELEASE_MS_CENTER 400.0f

#define ENV_MS_MAX ATTACK_MS_MAX + HOLD_MS_MAX + DECAY_MS_MAX + RELEASE_MS_MAX

// LFO

#define LFO_HZ_MIN 0.01f
#define LFO_HZ_MAX 25.0f
#define LFO_HZ_CENTER 3.0f
#define LFO_HZ_DEFAULT 1.0f

#define LFO_CENTER_MIN 0.01f
#define LFO_CENTER_MAX 0.99f
#define LFO_CENTER_DEFAULT 0.5f

#define LFO_MIDPOINT_MIN 0.01f
#define LFO_MIDPOINT_MAX 0.95f
#define LFO_MIDPOINT_DEFAULT 0.5f

// filter

#define CUTOFF_HZ_MIN 20.0f
#define CUTOFF_HZ_MAX 20000.0f
#define CUTOFF_HZ_CENTER 1000.0f
#define CUTOFF_HZ_DEFAULT 1200.0f

#define RESONANCE_MIN 0.1f
#define RESONANCE_MAX 10.0f
#define RESONANCE_CENTER 1.0f
#define RESONANCE_DEFAULT 0.5f

// saturation

#define SAT_COEFF_MIN 1.0f
#define SAT_COEFF_MAX 20.0f
#define SAT_COEFF_CENTER 5.0f
#define SAT_COEFF_DEFAULT 4.5f

#define SAT_DRIVE_MIN 1.0f
#define SAT_DRIVE_MAX 10.0f
#define SAT_DRIVE_CENTER 3.0f
#define SAT_DRIVE_DEFAULT 1.3f

#define CONTROL_LENGTH_DEFAULT 0.5f
#define CONTROL_ANGLE_DEFAULT MathConstants<float>::pi / 4.0f

#define QUICK_KILL_MS 2.5f

#define PERLIN_OCTAVES_DEFAULT 2
#define PERLIN_FREQ_DEFAULT 1.0f
#define PERLIN_LAC_DEAULT 2.0f

using frange = NormalisableRange<float>;
namespace IDs {
#define DECLARE_ID(name) const juce::Identifier name(#name);
DECLARE_ID(ELECTRUM_STATE)

// oscillator
DECLARE_ID(oscillatorPos)
DECLARE_ID(oscillatorLevel)
DECLARE_ID(oscillatorPan)
DECLARE_ID(oscillatorCoarseTune)
DECLARE_ID(oscillatorFineTune)

// envelope
DECLARE_ID(attackMs)
DECLARE_ID(attackCurve)
DECLARE_ID(holdMs)
DECLARE_ID(decayMs)
DECLARE_ID(decayCurve)
DECLARE_ID(sustainLevel)
DECLARE_ID(velocityTracking)
DECLARE_ID(releaseMs)
DECLARE_ID(releaseCurve)

// LFO
DECLARE_ID(lfoFreq)
DECLARE_ID(lfoCenterX)
DECLARE_ID(lfoMidpointA)
DECLARE_ID(lfoMidpointB)
DECLARE_ID(lfoGateTrigger)

// filter
DECLARE_ID(filterType)
DECLARE_ID(filterCutoff)
DECLARE_ID(filterResonance)
DECLARE_ID(filterMix)
DECLARE_ID(filterTracking)

// saturation
DECLARE_ID(saturationType)
DECLARE_ID(saturationCoeff)
DECLARE_ID(saturationDrive)
DECLARE_ID(saturationMix)

// perlin noise generaion
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
DECLARE_ID(lfoSource)

const std::vector<Identifier> ElectrumIDs = {oscillatorPos,
                                             oscillatorLevel,
                                             oscillatorPan,

                                             oscillatorCoarseTune,
                                             oscillatorFineTune,

                                             attackMs,
                                             attackCurve,
                                             holdMs,

                                             decayMs,
                                             decayCurve,
                                             sustainLevel,
                                             velocityTracking,

                                             releaseMs,
                                             releaseCurve,
                                             lfoFreq,
                                             lfoCenterX,
                                             lfoMidpointA,
                                             lfoMidpointB,
                                             lfoGateTrigger,

                                             filterType,
                                             filterCutoff,
                                             filterResonance,
                                             filterMix,
                                             filterTracking,

                                             saturationType,
                                             saturationCoeff,
                                             saturationDrive,
                                             saturationMix,

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
                                             envSource};

const std::vector<Identifier> DestinationIDs = {
    oscillatorPos,      oscillatorLevel, oscillatorPan,   oscillatorCoarseTune,
    oscillatorFineTune, filterCutoff,    filterResonance, filterMix,
    filterTracking,     saturationCoeff, saturationDrive, saturationMix};
#undef DECLARE_ID

#define NUM_DESTINATIONS 22

const StringArray filterTypes = {"Low Pass 12", "High Pass 12"};
const StringArray satTypes = {"Soft 1", "Soft 2", "Soft 3", "Soft 4"};
struct ParamInfoStrings
{
  String shortName;
  String longName;
  String desc;
};

const std::unordered_map<String, ParamInfoStrings> paramDisplayNames = {
    // oscillators
    {oscillatorPos.toString(),
     {"Osc. pos.", "Wavetable Oscillator Position",
      "Current position (range 0-1) in this oscillator's set of wavetables"}},
    {oscillatorLevel.toString(),
     {"Osc. level", "Oscillator level", "The oscillator's output level"}},
    {oscillatorPan.toString(),
     {"Osc. pan", "Oscillator pan", "The oscillator's position in the stereo field"}},
    {oscillatorCoarseTune.toString(),
     {"Osc. coarse tune", "Oscillator coarse tuning", "Coarse pitch adjustment"}},
    {oscillatorFineTune.toString(),
     {"Osc. fine tune", "Oscillator fine tuning", "Fine pitch adjustment"}},
    // perlin
    {perlinFreq.toString(),
     {"Freq.", "Perlin Noise Frequency", "How quickly the Perlin noise engine advances"}},
    {perlinOctaves.toString(),
     {"Octaves", "Perlin Noise Octaves",
      "The number of layers used by the Perlin noise algorithm"}},
    {perlinLacunarity.toString(),
     {"Lac.", "Perlin Noise Lacunarity",
      "The relative increase in frequency for each octave of Perlin noise"}},
    // envelopes
    {attackMs.toString(),
     {"Atk.", "Envelope attack time",
      "Attack: number of miliseconds between a note beginning and the envelope "
      "reaching its full lever"}},
    {attackCurve.toString(),
     {"AtkCrv", "Envelope attack curve",
      "Attack curve: this value corresponds to the level of the curve's "
      "midpoint"}},

    {holdMs.toString(),
     {"Hold", "Peak hold time",
      "Hold: the number of miliseconds for which the envelope with rest at max "
      "lever after the attack phase is over"}},
    {decayMs.toString(),
     {"Decay", "Envelope decay time",
      "Decay: time from the envelope hitting its peak and reaching its sustain "
      "level"}},
    {decayCurve.toString(),
     {"DcyCrv", "Envelope decay curve",
      "Decay curve: this value corresponds to the level of the curve's "
      "midpoint"}},

    {sustainLevel.toString(),
     {"Sustain", "Envelope sustain level",
      "Sustain: the level which the envelope maintains after the decay stage "
      "has finished and before the envelope's gate has ended"}},

    {velocityTracking.toString(),
     {"Velocity Tracking", "Envelope velocity tracking level",
      "How much the overall level of an envelope should be modulated by the "
      "current MIDI note's velocity"}},

    {releaseMs.toString(),
     {"Release", "Envelope release time",
      "Release: the time from the gate ending to the envelope output reaching "
      "0"}},
    {releaseCurve.toString(),
     {"RlsCrv", "Envelope release curve",
      "Release curve: this value corresponds to the level of the curve's "
      "midpoint"}},
    // LFO
    {lfoFreq.toString(), {"Freq", "LFO Frequency", "The speed of this LFO"}},
    {lfoCenterX.toString(),
     {"Center", "LFO peak point",
      "The phase in the LFO's cycle at which it reaches its peak level"}},
    {lfoMidpointA.toString(),
     {"A. Curve", "LFO attack curve", "The curve at which this LFO approaches its peak"}},
    {lfoMidpointB.toString(),
     {"R. Curve", "LFO release curve", "The curve at which this LFO approaches  zero"}},
    {lfoGateTrigger.toString(),
     {"Trig.", "LFO trigger", "Whether this LFO shoud be restarted by the MIDI gate"}},
    // Filter
    {filterType.toString(),
     {"Type", "Filter type", "Filter type: the type of filter for the main synth voices"}},
    {filterCutoff.toString(),
     {"Cutoff", "Filter cutoff frequency", "Filter cutoff: the main filter's cutoff frequency"}},
    {filterResonance.toString(),
     {"Resonance", "Filter resonance",
      "Filter resonance: percentage of this filter type's max resonance to "
      "use"}},
    {filterMix.toString(),
     {"Mix", "Filter wet/dry mix",
      "Filter mix: the proportion of filtered signal to original signal for "
      "the output"}},
    {filterTracking.toString(),
     {"Tracking", "Filter key tracking",
      "Filter key tracking: how much the note's fundamental pitch should "
      "change the filter offset"}},
    // Saturation
    {saturationType.toString(), {"Type", "Sat. type", "The type of saturation or clipping"}},
    {saturationCoeff.toString(),
     {"Strength", "Sat. coefficient", "Control the intensity of the saturation effect"}},
    {saturationMix.toString(),
     {"Mix", "Sat. wet/dry mix", "The proportion of the saturated signal to the original"}},
    {saturationDrive.toString(),
     {"Drive", "Sat. overdrive", "The amount of overdrive gain for the saturation input"}}};

inline String getParamName(const String &paramID, bool longName = false)
{
  String safeParamID = StringUtil::removeTrailingNumbers(paramID);
  auto it = paramDisplayNames.find(safeParamID);
  if (it != paramDisplayNames.end())
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
inline frange getResonanceRange()
{
  frange range(RESONANCE_MIN, RESONANCE_MAX, 0.00001f);
  range.setSkewForCentre(RESONANCE_CENTER);
  return range;
}

inline frange getLfoFreqRange()
{
  frange range(LFO_HZ_MIN, LFO_HZ_MAX, 0.0001f);
  range.setSkewForCentre(LFO_HZ_CENTER);
  return range;
}

inline frange getSatCoeffRange()
{
  frange range(SAT_COEFF_MIN, SAT_COEFF_MAX, 0.0001f);
  range.setSkewForCentre(SAT_COEFF_CENTER);
  return range;
}
inline frange getSatDriveRange()
{
  frange range(SAT_DRIVE_MIN, SAT_DRIVE_MAX, 0.0001f);
  range.setSkewForCentre(SAT_DRIVE_CENTER);
  return range;
}

inline AudioProcessorValueTreeState::ParameterLayout createElectrumLayout()
{
  AudioProcessorValueTreeState::ParameterLayout layout;
  frange posRange(0.0f, 1.0f, 0.0001f);
  frange levelRange(0.0f, 1.0f, 0.0001f);
  frange panRange(0.0f, 1.0f, 0.00001f);
  frange coarseRange(COARSE_TUNE_MIN, COARSE_TUNE_MAX, 1.0f);
  frange fineRange(FINE_TUNE_MIN, FINE_TUNE_MAX, 0.0001f);
  // oscillator params
  for (int i = 0; i < NUM_OSCILLATORS; i++)
  {
    auto iStr = String(i);
    String positionId = oscillatorPos.toString() + iStr;
    String levelId = oscillatorLevel.toString() + iStr;
    String panId = oscillatorPan.toString() + iStr;
    String coarseId = oscillatorCoarseTune.toString() + iStr;
    String fineId = oscillatorFineTune.toString() + iStr;
    auto levelName = getParamName(positionId, true);
    auto positionName = getParamName(levelId, true);
    auto panName = getParamName(panId, true);
    auto coarseName = getParamName(coarseId, true);
    auto fineName = getParamName(fineId, true);
    layout.add(
        std::make_unique<AudioParameterFloat>(positionId, positionName, posRange, OSC_POS_DEFAULT));
    layout.add(
        std::make_unique<AudioParameterFloat>(levelId, levelName, levelRange, OSC_LEVEL_DEFAULT));
    layout.add(std::make_unique<AudioParameterFloat>(panId, panName, panRange, 0.5f));
    layout.add(std::make_unique<AudioParameterFloat>(coarseId, coarseName, coarseRange, 0.0f));
    layout.add(std::make_unique<AudioParameterFloat>(fineId, fineName, fineRange, 0.0f));
  }
  // envelopes
  frange curveRange(ENV_CURVE_MIN, ENV_CURVE_MAX, 0.0001f);
  auto atkRange = getAttackRange();
  auto holdRange = getHoldRange();
  auto decayRange = getDecayRange();
  auto releaseRange = getReleaseRange();
  frange velTrackingRange(0.0f, 1.0f, 0.0001f);
  for (int i = 0; i < NUM_ENVELOPES; i++)
  {
    // attack MS
    auto iStr = String(i);
    String aMsID = attackMs.toString() + iStr;
    layout.add(std::make_unique<AudioParameterFloat>(aMsID, getParamName(aMsID, true), atkRange,
                                                     ATTACK_MS_DEFAULT));
    String aCurveID = attackCurve.toString() + iStr;
    layout.add(std::make_unique<AudioParameterFloat>(aCurveID, getParamName(aCurveID, true),
                                                     curveRange, ENV_CURVE_DEFAULT));
    // hold Ms
    String holdID = holdMs.toString() + iStr;
    layout.add(std::make_unique<AudioParameterFloat>(holdID, getParamName(holdID, true), holdRange,
                                                     HOLD_MS_DEFAULT));
    // decay Ms
    String decayID = decayMs.toString() + iStr;
    layout.add(std::make_unique<AudioParameterFloat>(decayID, getParamName(decayID, true),
                                                     decayRange, DECAY_MS_DEFAULT));
    String dCurveID = decayCurve.toString() + iStr;
    layout.add(std::make_unique<AudioParameterFloat>(dCurveID, getParamName(dCurveID, true),
                                                     curveRange, ENV_CURVE_DEFAULT));
    // sustain level
    String sustainID = sustainLevel.toString() + iStr;
    layout.add(std::make_unique<AudioParameterFloat>(sustainID, getParamName(sustainID, true), 0.0f,
                                                     1.0f, SUSTAIN_LEVEL_DEFAULT));
    // velocity tracking
    String velTrackingID = velocityTracking.toString() + iStr;
    layout.add(std::make_unique<AudioParameterFloat>(
        velTrackingID, getParamName(velTrackingID, true), velTrackingRange, VEL_TRACKING_DEFAULT));
    // release Ms
    String releaseID = releaseMs.toString() + iStr;
    layout.add(std::make_unique<AudioParameterFloat>(releaseID, getParamName(releaseID, true),
                                                     releaseRange, RELEASE_MS_DEFAULT));
    String rCurveID = releaseCurve.toString() + iStr;
    layout.add(std::make_unique<AudioParameterFloat>(rCurveID, getParamName(rCurveID, true),
                                                     curveRange, ENV_CURVE_DEFAULT));
  }
  // LFOs
  frange lfoFreqRange = getLfoFreqRange();
  frange centerRange(LFO_CENTER_MIN, LFO_CENTER_MAX, 0.001f);
  frange midpointRange(LFO_MIDPOINT_MIN, LFO_MIDPOINT_MAX, 0.001f);
  for (int i = 0; i < NUM_LFOS; i++)
  {
    String iStr(i);
    String freqID = lfoFreq.toString() + iStr;
    layout.add(std::make_unique<AudioParameterFloat>(freqID, getParamName(freqID, true),
                                                     lfoFreqRange, LFO_HZ_DEFAULT));

    String centerID = lfoCenterX.toString() + iStr;
    layout.add(std::make_unique<AudioParameterFloat>(centerID, getParamName(centerID, true),
                                                     centerRange, LFO_CENTER_DEFAULT));

    String aID = lfoMidpointA.toString() + iStr;
    layout.add(std::make_unique<AudioParameterFloat>(aID, getParamName(aID, true), midpointRange,
                                                     LFO_MIDPOINT_DEFAULT));
    String bID = lfoMidpointB.toString() + iStr;
    layout.add(std::make_unique<AudioParameterFloat>(bID, getParamName(bID, true), midpointRange,
                                                     LFO_MIDPOINT_DEFAULT));
    String trigID = lfoGateTrigger.toString() + iStr;
    layout.add(std::make_unique<AudioParameterBool>(trigID, getParamName(trigID, true), false));
  }

  // filter params
  String fTypeID = filterType.toString();
  layout.add(
      std::make_unique<AudioParameterChoice>(fTypeID, getParamName(fTypeID, true), filterTypes, 0));
  String cutoffID = filterCutoff.toString();
  layout.add(std::make_unique<AudioParameterFloat>(cutoffID, getParamName(cutoffID, true),
                                                   getCutoffRange(), CUTOFF_HZ_DEFAULT));
  String resID = filterResonance.toString();
  layout.add(std::make_unique<AudioParameterFloat>(resID, getParamName(resID, true),
                                                   getResonanceRange(), RESONANCE_DEFAULT));
  String filterMixID = filterMix.toString();
  layout.add(std::make_unique<AudioParameterFloat>(filterMixID, getParamName(filterMixID, true),
                                                   0.0f, 1.0f, 1.0f));
  String trackingID = filterTracking.toString();
  layout.add(std::make_unique<AudioParameterFloat>(trackingID, getParamName(trackingID, true), 0.0f,
                                                   1.0f, 1.0f));
  // saturation params
  String sTypeID = saturationType.toString();
  layout.add(
      std::make_unique<AudioParameterChoice>(sTypeID, getParamName(sTypeID, true), satTypes, 0));
  String sCoeffID = saturationCoeff.toString();
  layout.add(std::make_unique<AudioParameterFloat>(sCoeffID, getParamName(sCoeffID, true),
                                                   getSatCoeffRange(), SAT_COEFF_DEFAULT));
  String driveID = saturationDrive.toString();
  layout.add(std::make_unique<AudioParameterFloat>(driveID, getParamName(driveID, true),
                                                   getSatCoeffRange(), SAT_DRIVE_DEFAULT));
  String satMixID = saturationMix.toString();
  layout.add(std::make_unique<AudioParameterFloat>(satMixID, getParamName(satMixID, true), 0.0f,
                                                   1.0f, 0.5f));
  // perlin noise params
  frange pFreqRange(0.25f, 30.0f, 0.0001f);
  pFreqRange.setSkewForCentre(5.0f);
  String pFreqID = perlinFreq.toString();
  String pFreqName = "Perlin Frequency";
  layout.add(
      std::make_unique<AudioParameterFloat>(pFreqID, pFreqName, pFreqRange, PERLIN_FREQ_DEFAULT));

  String pOctID = perlinOctaves.toString();
  String pOctName = "Perlin Noise Octaves";
  layout.add(std::make_unique<AudioParameterInt>(pOctID, pOctName, 1, 20, PERLIN_OCTAVES_DEFAULT));

  frange lacunarityRange(1.0f, 5.0f, 0.0001f);
  String pLacID = perlinLacunarity.toString();
  String pLacName = "Perlin Noise Lacunarity";
  layout.add(
      std::make_unique<AudioParameterFloat>(pLacID, pLacName, lacunarityRange, PERLIN_LAC_DEAULT));

  return layout;
}

} // namespace IDs

#endif

#include "Electrum/Identifiers.h"
#include <memory>
#include "juce_audio_basics/juce_audio_basics.h"
#include "juce_audio_processors/juce_audio_processors.h"

juce::StringArray getFilterTypeNames() {
  return {"Ladder Low Pass 1", "Ladder Low Pass 2"};
}
// helper
static void addFloatParam(apvts::ParameterLayout* layout,
                          const String& id,
                          const String& name,
                          frange_t range,
                          float fDefault) {
  juce::ParameterID pID{id, 1};
  layout->add(
      std::make_unique<juce::AudioParameterFloat>(pID, name, range, fDefault));
}

static const float subdivLengths[NUM_SUBDIVS] = {
    1.0f / 4.0f, 1.0f / 6.0f, 1.0f / 2.0f, 1.0f / 3.0f, 1.0f,
    2.0f / 3.0f, 2.0f,        4.0f,        8.0f};

float subdivToQuarterNotes(NoteSubdivE sd) {
  return subdivLengths[(int)sd];
}

NoteSubdivE quarterNotesToSubdiv(float val) {
  const float thresh = 0.1f;
  for (int i = 0; i < NUM_SUBDIVS; ++i) {
    auto diff = std::fabs(val - subdivLengths[i]);
    if (diff < thresh) {
      return (NoteSubdivE)i;
    }
  }
  return quarterNotesToSubdiv(snapToSubdiv(val));
}

float snapToSubdiv(float value) {
  float minDist = 50000000.0f;
  int nearestIdx = -1;
  for (int i = 0; i < NUM_SUBDIVS; ++i) {
    auto diff = std::fabs(value - subdivLengths[i]);
    if (diff < minDist) {
      minDist = diff;
      nearestIdx = i;
    }
  }
  jassert(nearestIdx != -1);
  return subdivLengths[nearestIdx];
}

//-----------------------------------------------------------------
apvts::ParameterLayout ID::getParameterLayout() {
  apvts::ParameterLayout layout;
  // add your parameters here
  frange_t posRange(0.0f, 1.0f, 0.0001f);
  frange_t levelRange(0.0f, 1.0f, 0.0001f);
  frange_t panRange(0.0f, 1.0f, 0.00001f);
  frange_t coarseRange(COARSE_TUNE_MIN, COARSE_TUNE_MAX, 1.0f);
  frange_t fineRange(FINE_TUNE_MIN, FINE_TUNE_MAX, 0.0001f);
  // oscillator params
  for (int i = 0; i < NUM_OSCILLATORS; i++) {
    auto iStr = String(i);
    String activeID = oscillatorActive.toString() + iStr;
    String waveIdxID = oscillatorWaveIndex.toString() + iStr;
    String positionId = oscillatorPos.toString() + iStr;
    String levelId = oscillatorLevel.toString() + iStr;
    String panId = oscillatorPan.toString() + iStr;
    String coarseId = oscillatorCoarseTune.toString() + iStr;
    String fineId = oscillatorFineTune.toString() + iStr;
    String activeName = "Osc " + iStr + " active";
    String waveIdxName = "Osc " + iStr + " wavetable ID";
    String levelName = "Osc " + iStr + " level";
    String positionName = "Osc " + iStr + " pos";
    String panName = "Osc " + iStr + " pan";
    String coarseName = "Coarse tune " + iStr;
    String fineName = "Fine tune " + iStr;
    juce::ParameterID activePID{activeID, 1};
    layout.add(std::make_unique<juce::AudioParameterBool>(activePID, activeName,
                                                          i < 1));
    juce::ParameterID wavePID{waveIdxID, 1};
    layout.add(std::make_unique<juce::AudioParameterInt>(
        wavePID, waveIdxName, 0, MAX_NUM_WAVETABLES, 0));

    addFloatParam(&layout, positionId, positionName, posRange, OSC_POS_DEFAULT);
    addFloatParam(&layout, levelId, levelName, levelRange, OSC_LEVEL_DEFAULT);
    addFloatParam(&layout, panId, panName, panRange, 0.5f);
    addFloatParam(&layout, coarseId, coarseName, coarseRange, 0.0f);
    addFloatParam(&layout, fineId, fineName, fineRange, 0.0f);
  }
  // envelope params
  frange_t curveRange(ENV_CURVE_MIN, ENV_CURVE_MAX);
  auto attackRange =
      rangeWithCenter(ATTACK_MS_MIN, ATTACK_MS_MAX, ATTACK_MS_CENTER);
  auto holdRange = rangeWithCenter(HOLD_MS_MIN, HOLD_MS_MAX, HOLD_MS_CENTER);
  auto decayRange =
      rangeWithCenter(DECAY_MS_MIN, DECAY_MS_MAX, DECAY_MS_CENTER);
  const float sustainCenter = juce::Decibels::decibelsToGain(-17.0f);
  frange_t sustainRange = rangeWithCenter(0.0f, 1.0f, sustainCenter);
  frange_t velTrackingRange(0.0f, 1.0f);
  auto releaseRange =
      rangeWithCenter(RELEASE_MS_MIN, RELEASE_MS_MAX, RELEASE_MS_CENTER);
  for (int i = 0; i < NUM_ENVELOPES; ++i) {
    String iStr(i);
    const String aMsID = attackMs.toString() + iStr;
    const String aMsName = "Env " + iStr + " attack";
    const String aCurveID = attackCurve.toString() + iStr;
    const String aCurveName = "Env " + iStr + " attack curve";
    const String hID = holdMs.toString() + iStr;
    const String hName = "Env " + iStr + " hold";
    const String dMsID = decayMs.toString() + iStr;
    const String dMsName = "Env " + iStr + " decay";
    const String dCurveID = decayCurve.toString() + iStr;
    const String dCurveName = "Env " + iStr + " decay curve";
    const String sID = sustainLevel.toString() + iStr;
    const String sName = "Env " + iStr + " sustain";
    const String rMsID = releaseMs.toString() + iStr;
    const String rMsName = "Env " + iStr + " release";
    const String rCurveID = releaseCurve.toString() + iStr;
    const String rCurveName = "Env " + iStr + " release curve";
    const String vID = velocityTracking.toString() + iStr;
    const String vName = "Env " + iStr + " vel. tracking";

    addFloatParam(&layout, aMsID, aMsName, attackRange, ATTACK_MS_DEFAULT);
    addFloatParam(&layout, aCurveID, aCurveName, curveRange, ENV_CURVE_DEFAULT);
    addFloatParam(&layout, hID, hName, holdRange, HOLD_MS_DEFAULT);
    addFloatParam(&layout, dMsID, dMsName, decayRange, DECAY_MS_DEFAULT);
    addFloatParam(&layout, dCurveID, dCurveName, curveRange, ENV_CURVE_DEFAULT);
    addFloatParam(&layout, sID, sName, sustainRange, SUSTAIN_LEVEL_DEFAULT);
    addFloatParam(&layout, rMsID, rMsName, releaseRange, RELEASE_MS_DEFAULT);
    addFloatParam(&layout, rCurveID, rCurveName, curveRange, ENV_CURVE_DEFAULT);
    addFloatParam(&layout, vID, vName, sustainRange, VEL_TRACKING_DEFAULT);
  }

  // filter params-----------------
  frange_t filterGainRange =
      rangeWithCenter(FILTER_DB_MIN, FILTER_DB_MAX, 0.0f);
  frange_t cutoffRange = rangeWithCenter(FILTER_CUTOFF_MIN, FILTER_CUTOFF_MAX,
                                         FILTER_CUTOFF_CENTER);
  frange_t resonanceRange =
      rangeWithCenter(FILTER_RES_MIN, FILTER_RES_MAX, FILTER_RES_DEFAULT);
  for (int i = 0; i < NUM_FILTERS; ++i) {
    String iStr(i);
    const String cutoffID = filterCutoff.toString() + iStr;
    const String cutoffName = "Filter " + iStr + " cutoff";
    const String resID = filterResonance.toString() + iStr;
    const String resName = "Filter " + iStr + " resonance";
    const String gainID = filterGainDb.toString() + iStr;
    const String gainName = "Filter " + iStr + " gain";
    const String activeID = filterActive.toString() + iStr;
    const String activeName = "Filter " + iStr + " active";
    const String typeID = filterType.toString() + iStr;
    const String typeName = "Filter " + iStr + " type";
    juce::ParameterID typePID{typeID, 1};
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        typePID, typeName, getFilterTypeNames(), 0));
    addFloatParam(&layout, cutoffID, cutoffName, cutoffRange, 1000.0f);
    addFloatParam(&layout, resID, resName, resonanceRange, FILTER_RES_DEFAULT);
    addFloatParam(&layout, gainID, gainName, filterGainRange, 0.0f);
    juce::ParameterID activePID{activeID, 1};
    layout.add(std::make_unique<juce::AudioParameterBool>(activePID, activeName,
                                                          i < 1));
    // routing countrols
    for (int o = 0; o < NUM_OSCILLATORS; ++o) {
      String oStr(o + 1);
      String routeID = "filterOsc" + oStr + "On" + iStr;
      String routeName = "Route osc. " + oStr + " to filter " + iStr;
      juce::ParameterID routePID{routeID, 1};
      layout.add(std::make_unique<juce::AudioParameterBool>(routePID, routeName,
                                                            i < 1));
    }
  }
  // LFO params--------------------------------------
  frange_t lfoHzRange = rangeWithCenter(LFO_HZ_MIN, LFO_HZ_MAX, LFO_HZ_CENTER);
  for (int i = 0; i < NUM_LFOS; ++i) {
    String iStr(i);
    const String freqID = lfoFrequencyHz.toString() + iStr;
    const String freqName = "LFO " + iStr + " speed (hz)";
    const String trigModeID = lfoTriggerMode.toString() + iStr;
    const String trigModeName = "LFO " + iStr + " trigger mode";
    const String syncID = lfoBeatSync.toString() + iStr;
    const String syncName = "LFO " + iStr + " beat sync";
    const String subdivID = lfoNoteSubdiv.toString() + iStr;
    const String subdivName = "LFO " + iStr + " speed (beat sync)";

    addFloatParam(&layout, freqID, freqName, lfoHzRange, LFO_HZ_CENTER);
    juce::ParameterID trigPID{trigModeID, 1};
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        trigPID, trigModeName, getTriggerModeNames(), 0));

    juce::ParameterID syncPID{syncID, 1};
    layout.add(
        std::make_unique<juce::AudioParameterBool>(syncPID, syncName, false));

    juce::ParameterID subdivPID{subdivID, 1};
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        subdivPID, subdivName, getNoteSubdivNames(), 4));
  }

  return layout;
}

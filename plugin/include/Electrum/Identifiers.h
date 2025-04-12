#pragma once
/* This namespace is were all the string identifiers for
 * parameters, components, and anything else should live.
 * Simply use the `DECLARE_ID` macro and never worry about
 * misspelling a string literal again.
 *
 * This translation unit is also responsible for the
 * `ID::getParameterLayout()` function which we use to set up
 * the AudioProcessorValueTreeState that runs everything
 * */
#include <juce_core/juce_core.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "juce_graphics/juce_graphics.h"
// typedefs bc some of these types are a mouthful
typedef juce::AudioProcessorValueTreeState apvts;
typedef std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
    slider_attach_ptr;
typedef std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>
    combo_attach_ptr;
typedef std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>
    button_attach_ptr;
typedef juce::NormalisableRange<float> frange_t;

typedef juce::Rectangle<float> frect_t;
typedef juce::Rectangle<int> irect_t;
typedef juce::File File;

// very helpful
inline frange_t rangeWithCenter(float min, float max, float center) {
  frange_t range(min, max);
  range.setSkewForCentre(center);
  return range;
}

// some type aliases to maybe type juce:: fewer times
typedef juce::String String;
typedef juce::Component Component;
typedef juce::ValueTree ValueTree;

// defines for param ranges
// Every Identifier for every parameter should be in here
#define NUM_OSCILLATORS 3
#define NUM_ENVELOPES 3
#define NUM_LFOS 3
#define NUM_FILTERS 2

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

#define FILTER_CUTOFF_MIN 20.0f
#define FILTER_CUTOFF_MAX 18000.0f
#define FILTER_CUTOFF_CENTER 1600.0f

#define FILTER_RES_MIN 0.0f
#define FILTER_RES_MAX 4.5f
#define FILTER_RES_DEFAULT 1.2f

#define DECLARE_ID(name) const juce::Identifier name(#name);

namespace ID {
// top level ID for the apvts
DECLARE_ID(ELECTRUM_STATE)

DECLARE_ID(ELECTRUM_MOD_TREE)
DECLARE_ID(ELECTRUM_MODULATION)
DECLARE_ID(modSourceID)
DECLARE_ID(modDestID)
DECLARE_ID(modDepth)

// wavetable osc stuff
DECLARE_ID(oscillatorPos)
DECLARE_ID(oscillatorActive)
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

// filter
DECLARE_ID(filterActive)
DECLARE_ID(filterCutoff)
DECLARE_ID(filterResonance)
DECLARE_ID(filterGainDb)

//--------
// patch metadata stuff
DECLARE_ID(PATCH_INFO)
DECLARE_ID(patchName)
DECLARE_ID(patchPath)
DECLARE_ID(patchAuthor)
DECLARE_ID(patchDesc)
DECLARE_ID(patchCategory)

// wave metadata stuff
DECLARE_ID(WAVE_INFO)
DECLARE_ID(waveName)
DECLARE_ID(wavePath)
DECLARE_ID(waveAuthor)
DECLARE_ID(waveCategory)
DECLARE_ID(waveStringData)

// Oscillator wave names
DECLARE_ID(oscWavePath)

apvts::ParameterLayout getParameterLayout();
}  // namespace ID

#undef DECLARE_ID

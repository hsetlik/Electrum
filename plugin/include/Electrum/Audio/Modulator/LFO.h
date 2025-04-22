#pragma once
#include "../AudioUtil.h"
#include "Electrum/Common.h"
#include "Electrum/Identifiers.h"
#include "juce_events/juce_events.h"

#define LFO_SIZE 2048

typedef std::array<float, LFO_SIZE> lfo_table_t;

// represents one editable point on the LFO
// a vector of these can be used to compute the table
struct lfo_handle_t {
  size_t tableIdx;
  float level;
};

struct lfo_bin_state {
  lfo_handle_t* leftHandle;
  lfo_handle_t* rightHandle;
  float t;
};

namespace LFO {
// converts the `lfo_handle_t` vector into a string to be saved/loaded
String stringEncode(std::vector<lfo_handle_t>& handles);
// parses the given string into a list of LFO handles
void stringDecode(const String& str, std::vector<lfo_handle_t>& dest);
// parses a list of handles into an actual LUT
void parseHandlesToTable(const std::vector<lfo_handle_t>& handles,
                         lfo_table_t& dest);
}  // namespace LFO

// the shared data for our LFOs, analogous to EnvelopeLUT
#define LFO_UPDATE_HZ 10
class LowFrequencyLUT : public juce::Timer, public juce::AsyncUpdater {
private:
  // have two of these such that we can always have one that's realtime ready
  lfo_table_t table1;
  lfo_table_t table2;
  lfo_table_t* tActive = &table1;
  lfo_table_t* tIdle = &table2;
  size_t currentLfoHash = 0;
  String currentLfoString = "";
  std::vector<lfo_handle_t> handles;

  bool needsData = true;

  float lfoHz = 0.05f;
  float phaseDelt = 0.00001f;

  float globalPhase = 0.0f;

  LFOTriggerE trigMode = LFOTriggerE::Global;

public:
  LowFrequencyLUT();
  bool wantsUpdate() const { return needsData; }
  void handleAsyncUpdate() override;
  void timerCallback() override;
  float getSample(float normPhase) const;
  float getGlobalPhase() const { return globalPhase; }
  // call this in per-block update
  void updateData(apvts& tree, int lfoIDX);
  // call this once per sample to advance the global phase
  void tick();
  void setHz(float freq) {
    lfoHz = freq;
    phaseDelt = (float)((double)lfoHz / SampleRate::get());
  }
  void setTriggerMode(float fTrigMode) {
    int iMode = (int)fTrigMode;
    trigMode = (LFOTriggerE)iMode;
  }

  float getHz() const { return lfoHz; }
  float getPhaseDelt() const { return phaseDelt; }
  LFOTriggerE getTriggerMode() const { return trigMode; }
};

//=============================================================

class VoiceLFO {
private:
  juce::Random rng;
  LowFrequencyLUT* const lut;
  float phase = 0.0f;
  float lastOutput = 0.0f;
  float _getNext();

public:
  VoiceLFO(LowFrequencyLUT* l);
  void tick();
  void gateStarted();
  float getCurrentSample() const { return lastOutput; }
  float getCurrentPhase() const;
};

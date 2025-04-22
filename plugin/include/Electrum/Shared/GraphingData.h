#pragma once
#include "Electrum/Audio/Wavetable.h"
#include "Electrum/Common.h"
#include "Electrum/Identifiers.h"
#include "juce_core/juce_core.h"
#include "juce_core/system/juce_PlatformDefs.h"

// handy type aliases
typedef std::atomic<float> float_at;
typedef std::atomic<int> int_at;
typedef std::atomic<uint32_t> uint32_at;
typedef std::atomic<bool> bool_at;

#define WAVE_GRAPH_POINTS 70

typedef std::array<float, WAVE_GRAPH_POINTS> graph_wave_t;

class AtomicIntStack {
private:
  std::array<int_at, 32> data = {};
  int_at head;

public:
  AtomicIntStack() : head(-1) {}
  bool empty() const { return head < 0; }
  int top() const { return data[(size_t)head].load(); }
  void pop() { head--; }
  void push(int val) {
    if (head == 32) {
      head = 0;
    }
    data[(size_t)++head] = val;
  }
};

/* This class atomically stores data
 * about the audio thread that our
 * various GUI components need to access
 * */
class GraphingData : public juce::Timer {
private:
  // just in case we need to lock maybe
  juce::CriticalSection criticalSection;
  int_at voicesState;
  int_at newestVoice;
  AtomicIntStack voiceIndeces;

  std::array<float_at, NUM_OSCILLATORS> newestOscPositions;
  std::array<float_at, NUM_ENVELOPES> newestEnvLevels;
  std::array<float_at, NUM_LFOS> newestLfoLevels;

  // keep track of when we want updates
  bool_at updateRequested;

  // string-encoded versions of our wavetables
  // that our graphics code can (god willing) safely
  // access
  std::array<String, NUM_OSCILLATORS> waveStrings;
  bool_at needsWaveStrings;
  bool_at waveStringsHaveChanged;

public:
  GraphingData();
  void timerCallback() override { updateRequested = true; }
  bool wantsUpdate() const {
    return updateRequested.load() && newestVoice != -1;
  }
  void updateFinished() {
    _notifyListeners();
    updateRequested = false;
    waveStringsHaveChanged = false;
    needsWaveStrings = false;
  }
  // call this so we can keep track of which voice to be tracking from
  void voiceStarted(int idx);
  void voiceEnded(int idx);
  int getNewestVoiceIndex() const { return newestVoice.load(); }
  // call these on the 'newestVoice' at update time to do the actual
  // data copying
  void updateOscPos(int oscID, float value) {
    newestOscPositions[(size_t)oscID] = value;
  }
  float getOscPos(int oscID) const {
    return newestOscPositions[(size_t)oscID].load();
  }
  void updateEnvLevel(int envID, float value) {
    newestEnvLevels[(size_t)envID] = value;
  }
  float getEnvLevel(int envID) const {
    return newestEnvLevels[(size_t)envID].load();
  }
  void updateLFOLevel(int lfoID, float value) {
    newestLfoLevels[(size_t)lfoID] = value;
  }
  float getLFOLevel(int lfoID) const {
    return newestOscPositions[(size_t)lfoID].load();
  }

  // wave string stuff
  bool needsWavetableData() const { return needsWaveStrings; }
  bool wavetablesChanged() const { return waveStringsHaveChanged; }
  void requestWavetableString(int) { needsWaveStrings = true; }
  void updateWavetableString(const String& wave, int oscID);
  String getWavetableString(int oscID) const noexcept {
    return waveStrings[(size_t)oscID];
  }

  // Graphing components should inherit from this to get
  // updates-----------------------
  class Listener {
  public:
    Listener() = default;
    virtual ~Listener() {}
    virtual void graphingDataUpdated(GraphingData* gd) {
      juce::ignoreUnused(gd);
    }
  };
  //-----------------------
  void addListener(Listener* l) { graphListeners.push_back(l); }
  void removeListener(Listener* l);

private:
  std::vector<Listener*> graphListeners = {};
  void _notifyListeners();
  bool _isVoiceActive(int idx);
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GraphingData)
};

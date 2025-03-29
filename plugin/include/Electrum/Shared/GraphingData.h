#pragma once
#include "Electrum/Audio/Wavetable.h"
#include "Electrum/Common.h"
#include "Electrum/Identifiers.h"
#include "juce_core/juce_core.h"

// handy type aliases
typedef std::atomic<float> float_at;
typedef std::atomic<int> int_at;
typedef std::atomic<uint32_t> uint32_at;
typedef std::atomic<bool> bool_at;

#define WAVE_GRAPH_POINTS 512
typedef std::array<float, WAVE_GRAPH_POINTS> single_wave_norm_t;
struct WavetableGraphingPoints {
  std::vector<single_wave_norm_t> waves = {};
  int oscID = 0;
};

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
  int_at voicesState;
  int_at newestVoice;
  AtomicIntStack voiceIndeces;

  std::array<float_at, NUM_OSCILLATORS> newestOscPositions;
  std::array<float_at, NUM_ENVELOPES> newestEnvLevels;

  // keep track of when we want updates
  bool_at updateRequested;

  // this graphing data can be non-atomic
  bool_at graphPointsReady = false;

public:
  WavetableGraphingPoints graphPoints[NUM_OSCILLATORS];
  GraphingData();
  void timerCallback() override;

  bool wantsGraphPoints() const { return !graphPointsReady.load(); }
  void graphPointsLoaded() { graphPointsReady = true; }
  bool wantsUpdate() const {
    return updateRequested.load() && newestVoice != -1;
  }
  void updateFinished() {
    updateRequested = false;
    _notifyListeners();
  }
  // call this so we can keep track of which voice to be tracking from
  void voiceStarted(int idx);
  void voiceEnded(int idx);
  int getNewestVoiceIndex() const { return newestVoice.load(); }
  // call these on the 'newestVoice' at update time to do the actual
  // data copying
  void updateOscPos(int oscID, float value, bool notify = true) {
    newestOscPositions[(size_t)oscID] = value;
    if (notify)
      _notifyListeners();
  }
  float getOscPos(int oscID) const {
    return newestOscPositions[(size_t)oscID].load();
  }
  void updateEnvLevel(int envID, float value, bool notify = true) {
    newestEnvLevels[(size_t)envID] = value;
    if (notify)
      _notifyListeners();
  }
  // update the graphing point data
  void updateGraphPoints(Wavetable* wt, int oscID, bool notify = true);
  // Graphing components should inherit from this to get
  // updates-----------------------
  class Listener {
  public:
    Listener() = default;
    virtual ~Listener() {}
    virtual void graphingDataUpdated(GraphingData* gd) {
      juce::ignoreUnused(gd);
    }
    // this gets a special callback to avoid re-loading waves every time the
    // position changes
    virtual void wavePointsUpdated(GraphingData* gd, int oscID) {
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
};

#include "Electrum/Shared/GraphingData.h"
#include "Electrum/Audio/AudioUtil.h"

GraphingData::GraphingData()
    : voicesState(0),
      newestVoice(0),
      updateRequested(false),
      needsWaveStrings(true),
      waveStringsHaveChanged(false) {
  for (int i = 0; i < NUM_ENVELOPES; i++) {
    newestEnvLevels[(size_t)i] = 0.0f;
  }

  for (int i = 0; i < NUM_OSCILLATORS; i++) {
    newestOscPositions[(size_t)i] = OSC_POS_DEFAULT;
  }
  startTimerHz(30);
}

void GraphingData::updateWavetableString(const String& wave, int oscID) {
  waveStrings[(size_t)oscID] = wave;
  waveStringsHaveChanged = true;
  DLog::log("Updated wavetable string for osc " + String(oscID));
}

//-----------------------
void GraphingData::voiceStarted(int idx) {
  newestVoice = idx;
  voiceIndeces.push(idx);
  const int mask = 1 << idx;
  voicesState |= mask;
}

void GraphingData::voiceEnded(int idx) {
  if (idx == newestVoice) {
    voiceIndeces.pop();
    if (!voiceIndeces.empty()) {
      // make sure that the new 'newestVoice'
      // is actually still active
      newestVoice = voiceIndeces.top();
      while (!_isVoiceActive(newestVoice.load()) && !voiceIndeces.empty()) {
        voiceIndeces.pop();
        newestVoice = voiceIndeces.top();
      }
      if (voiceIndeces.empty())
        newestVoice = -1;
    }
  }
  const int mask = ~(1 << idx);
  voicesState &= mask;
}

bool GraphingData::_isVoiceActive(int idx) {
  const int mask = 1 << idx;
  return (voicesState & mask) > 0;
}

void GraphingData::_notifyListeners() {
  for (auto* l : graphListeners) {
    l->graphingDataUpdated(this);
  }
}

void GraphingData::removeListener(Listener* l) {
  for (auto it = graphListeners.begin(); it != graphListeners.end(); it++) {
    auto ptr = *it;
    if (ptr == l) {
      graphListeners.erase(it);
      return;
    }
  }
}

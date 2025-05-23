#pragma once
#include "../Identifiers.h"
#include "Electrum/Shared/CommonAudioData.h"
#include "Electrum/Shared/FileSystem.h"
#include "GraphingData.h"
#include "juce_audio_basics/juce_audio_basics.h"
#include "juce_data_structures/juce_data_structures.h"
String paramIDForModDest(int destID);

//======================================
// stuff for handling sync with the plugin host's tempo/playhead info
struct rhythm_state_t {
  int timeSigNum = 4;
  int timeSigDen = 4;
  double bpm = 120.0;
};
//======================================

typedef std::array<std::array<float, MOD_DESTS>, MOD_SOURCES> depth_array_t;
typedef std::array<std::array<bool, MOD_DESTS>, MOD_SOURCES> toggle_array_t;

struct mod_src_t {
  int source;
  float depth;
};

class ModMap {
private:
  depth_array_t depthArr;
  toggle_array_t boolArr;
  void clearBoolGrid();

public:
  ModMap();
  void updateMap(ValueTree modTree);
  bool modExists(int src, int dest) const;
  int numSourcesOnDest(int dest) const;
  bool destInUse(int dest) const { return numSourcesOnDest(dest) > 0; }
  std::vector<mod_src_t> getSourcesFor(int dest);
  void getSourcesSafe(mod_src_t* arr, int* numSources, int destID) const;
};

//===========================================================

struct timed_midi_msg {
  int timestamp;
  juce::MidiMessage message;
};

//===========================================================
// APVTS subclass to handle all manner of things

class ElectrumState : public apvts {
private:
  frange_t modDestRanges[MOD_DESTS];

  // controller state stuff
  bool sustainPedal = false;
  float modWheelValue = 0.0f;
  std::array<int, NUM_OSCILLATORS> lastWaveIndices;

  // time signature/tempo stuff

  // this sets whether we should check for tempo information, it should be set
  // based on whether the host provides it during the first callback
  bool shouldCheckTempo = true;
  // will we ask for tempo info from the `AudioProcessor` on the next
  // processBlock call
  bool needsRhythmState = true;
  rhythm_state_t rhythmState;

public:
  ElectrumState(juce::AudioProcessor& proc, juce::UndoManager* undo);
  inline ValueTree getModulationTree() {
    return state.getChildWithName(ID::ELECTRUM_MOD_TREE);
  }
  // helper for moduation
  float getModulatedDestValue(int destID, float baseValue, float modNorm) const;
  // our components should call these
  void setModulation(int src, int dest, float depth);
  void removeModulation(int src, int dest);
  // these are helpful for components to check if they're
  // up to date
  bool modulationExists(int src, int dest);
  float modulationDepth(int src, int dest);

  float getFloatParamValue(const String& id) {
    return getRawParameterValue(id)->load();
  }

  // for setting up the tempo information
  bool wantsPlayHeadUpdate() const {
    return shouldCheckTempo && needsRhythmState;
  }
  void updatePlayHead(juce::AudioPlayHead* ph);
  // convert a value in quarter notes to a frequency in hertz at the current BPM
  float noteSubdivToHz(float quarterNotes) const;
  //-----------------------------------------------
  bool getSustainPedal() const { return sustainPedal; }
  void setSustainPedal(bool pedalDown) { sustainPedal = pedalDown; }
  void setModWheel(float val) { modWheelValue = val; }

  // modulation data for the audio thread to access
  ModMap modulations;
  // atomic data holder for our GUI graphing
  GraphingData graph;
  // manager for all our patch and wave files
  ElectrumUserLib userLib;
  // the shared LUTs and such four our voices
  CommonAudioData audioData;

  void updateLFOString(const String& shapeString, int lfoID);

  void updateCommonAudioData();
  void ensureLFOTree();

private:
  ValueTree findTreeForRouting(const ValueTree& modTree, int src, int dest);
};

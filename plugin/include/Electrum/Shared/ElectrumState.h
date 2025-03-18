#pragma once
#include "../Identifiers.h"
#include "juce_data_structures/juce_data_structures.h"

// structs for modulations

enum mod_source { Env1, Env2, Env3, ModWheel, Velocity };

#define MOD_SOURCES 5

enum mod_dest {
  osc1Coarse,
  osc1Fine,
  osc1Pos,
  osc1Level,
  osc1Pan,
  osc2Coarse,
  osc2Fine,
  osc2Pos,
  osc2Level,
  osc2Pan
};

#define MOD_DESTS 10

typedef std::array<std::array<float, MOD_SOURCES>, MOD_DESTS> depth_array_t;
typedef std::array<std::array<bool, MOD_SOURCES>, MOD_DESTS> toggle_array_t;

class ModMap {
private:
  depth_array_t depthArr;
  toggle_array_t boolArr;
  void clearBoolGrid();

public:
  ModMap();
  void updateMap(ValueTree& modTree);
};

//===========================================================
// APVTS subclass to handle all manner of things

class ElectrumState : public apvts {
private:
public:
  ElectrumState(juce::AudioProcessor& proc, juce::UndoManager* undo);
};

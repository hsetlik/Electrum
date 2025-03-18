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
  std::vector<mod_src_t> getSourcesFor(int dest);
};

//===========================================================
// APVTS subclass to handle all manner of things

class ElectrumState : public apvts {
private:
public:
  ElectrumState(juce::AudioProcessor& proc, juce::UndoManager* undo);
  inline ValueTree getModulationTree() {
    return state.getChildWithName(ID::ELECTRUM_MOD_TREE);
  }
  // our components should call these
  void setModulation(int src, int dest, float depth);
  void removeModulation(int src, int dest);
  ModMap modulations;

private:
  ValueTree findTreeForRouting(const ValueTree& modTree, int src, int dest);
};

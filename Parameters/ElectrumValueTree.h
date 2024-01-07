#pragma once
#include "../Audio/Modulators/Perlin.h"
#include "../GUI/Color.h"
#include "AudioSystem.h"
#include "ElectrumAudioData.h"
#include "ElectrumVoicesState.h"
#include "Identifiers.h"
#include "ParameterWatcher.h"
#include <stack>

/*
 * This represents a single modulation with a sourceID string and a depth value
 * in the range -1.0-1.0
 * */
struct Modulation
{
  String sourceID;
  float depth;
  Modulation(const String &s, float d) : sourceID(s), depth(d) {}
};
/*
 * this represents a list of all the mod sources assigned to a single mod
 * destination
 * */
class ModulationDestList
{
private:
  OwnedArray<Modulation> mods;

public:
  String destID;
  ModulationDestList(const String &id = "OscPosition0");
  ModulationDestList(const String &id, ValueTree &tree);

  OwnedArray<Modulation> &getMods() { return mods; }

  void addMod(const String &srcID, float depth);
  void removeMod(const String &srcID);
  void setMod(const String &srcID, float depth);
  bool hasMod(const String &srcID);

  void clearMods() { mods.clear(); }
};

class ModDestMap
{
private:
  std::array<ModulationDestList, NUM_DESTINATIONS> modArr;

public:
  ModDestMap();
  void loadFromTree(ValueTree &modTree);
  OwnedArray<Modulation> *getModsFor(const String &destID);
};
//============================================================================================
class EVT : public APVTS::Listener
{
private:
  static ValueTree createModulationsTree()
  {
    ValueTree tree(IDs::ELECTRUM_MODULATIONS);
    return tree;
  }
  static ValueTree createModulationTree(const String &source, const String &dest, float depth)
  {
    ValueTree tree(IDs::MODULATION);
    tree.setProperty(IDs::modulationSource, source, nullptr);
    tree.setProperty(IDs::modulationDest, dest, nullptr);
    tree.setProperty(IDs::modulationDepth, depth, nullptr);
    return tree;
  }
  //=================================================================================
  ValueTree getModulationsTree();
  std::vector<ValueTree> getModulations();
  ValueTree getModulation(const String &source, const String &dest);
  bool sourceIsInUse(const String &source);
  //========================================================================================
  // state
  std::unique_ptr<ElectrumAudioData> audioData;
  APVTS coreTree;
  ParameterWatcher paramWatcher;
  std::atomic<bool> sustainPedalOn;
  std::atomic<float> modWheelValue;
  std::atomic<float> pitchBendValue;
  String currentFilterType = IDs::filterTypes[0];
  int envsInUse;
  //========================================================================================
  // perlin stuff
  PerlinGenerator perlin;
  std::atomic<float> lastPerlinVal;
  //========================================================================================
  // graphical feedback stuff
  std::atomic<bool> editorOpen;
  std::atomic<int> voicesState;
  std::atomic<int> newestVoice;
  std::array<std::atomic<float>, NUM_ENVELOPES> newestEnvLevels;
  std::array<std::atomic<float>, NUM_OSCILLATORS> newestOscPositions;
  std::array<std::atomic<float>, NUM_LFOS> newestLFOLevels;
  std::stack<int> voiceIndeces;

  // TODO: figure out a way to store the APVTS and the waves in a single XML file in a way which can
  // be saved and loaded to EVT & ElectrumAudioData objects

public:
  EVT(AudioProcessor &proc, UndoManager *undo, const Identifier &valueTreeType);
  ~EVT() override;

  String getCurrentFilterType() { return currentFilterType; }

  ElectrumAudioData *getAudioData() { return audioData.get(); }
  // helper functions for accesing the underlying atomic values. unchecked!
  float getFloatParamValue(const String &id);

  int getIntParamValue(const String &id);

  int getChoiceParamValue(const String &id);

  AudioParameterFloat *getFloatParamPtr(const String &id);

  // Since the perlin noise generator is shared by all voices, we handle it here
  // updates the perlin noise generator with any UI changes

  // advances the perlin generator for the next sample
  void tickPerlinForSample() { lastPerlinVal = perlin.getNextValue(); }
  // gets the current output of the perlin generator
  float perlinValue() const { return lastPerlinVal.load(); }

  float getOscillatorValue(int idx, float phase, float tablePos, double freq, double sampleRate)
  {
    return audioData->getOscillatorValue(idx, phase, tablePos, freq, sampleRate);
  }
  APVTS *getAPVTS() { return &coreTree; }
  // Modulation info stuff
  // use this to add or update the value of a modulation
  void setModulation(const String &source, const String &dest, float depth);

  // check if a given modulation exists
  bool modulationExists(const String &source, const String &dest);

  // removes a modulation routing
  void removeModulation(const String &src, const String &dest);
  // updates the AudioData with the current envelope parameters
  void updateEnvelopesForBlock();
  void updatePerlinForBlock();
  void updateLFOsForBlock();

  bool envIsInUse(int idx) { return envsInUse & (1 << idx); }

  void setSustainPedal(bool shouldBeOn) { sustainPedalOn = shouldBeOn; }

  bool getSustainPedal() { return sustainPedalOn.load(); }

  void setModWheel(float val) { modWheelValue = val; }

  float getModWheel() { return modWheelValue.load(); }

  void setPitchBend(float val) { pitchBendValue = val; }

  float getPitchBend() { return pitchBendValue.load(); }
  // load the modulations to a structure we can access in a thread-safe way
  void loadModulationData(ModDestMap &modMap);
  // APVTS::Listener override
  void parameterChanged(const String &paramID, float value) override;

  void editorOpened() { editorOpen = true; }
  void editorClosed() { editorOpen = false; }
  bool isEditorOpen() const { return editorOpen.load(); }

  // voice logic
  void startVoice(int idx)
  {
    int result = ElectrumVoicesState::startVoice(voicesState.load(), idx);
    voicesState = result;
    newestVoice = idx;
    voiceIndeces.push(idx);
  }
  void endVoice(int idx)
  {
    if (idx == newestVoice.load())
    {
      voiceIndeces.pop();
      if (!voiceIndeces.empty())
        newestVoice = voiceIndeces.top();
    }
    int result = ElectrumVoicesState::endVoice(voicesState.load(), idx);
    voicesState = result;
  }
  bool anyVoicesActive()
  {
    int data = voicesState.load();
    return data != 0;
  }
  bool isVoiceActive(int idx)
  {
    return ElectrumVoicesState::isVoiceActive(voicesState.load(), idx);
  }
  int currentNewestVoice() { return newestVoice.load(); }

  // getters/setters for GUI dieplays
  void setLeadingVoiceEnvLevel(int idx, float value) { newestEnvLevels[(size_t)idx] = value; }

  float getLeadingVoiceEnvLevel(int idx) { return newestEnvLevels[(size_t)idx].load(); }

  float getLeadingVoiceLFOLevel(int idx)
  {
    if (anyVoicesActive())
      return newestLFOLevels[(size_t)idx].load();
    return 0.0f;
  }

  void setLeadingVoiceLFOLevel(int idx, float value) { newestLFOLevels[(size_t)idx] = value; }
  void setLeadingVoiceOscPosition(int idx, float value) { newestOscPositions[(size_t)idx] = value; }
  float getLeadingVoiceOscPosition(int idx) { return newestOscPositions[(size_t)idx].load(); }
};

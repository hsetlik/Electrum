
#include "Electrum/Shared/ElectrumState.h"
#include "Electrum/Audio/AudioUtil.h"
#include "Electrum/Audio/Modulator/LFO.h"
#include "Electrum/Identifiers.h"
#include "Electrum/Shared/FileSystem.h"
#include "juce_audio_basics/juce_audio_basics.h"
#include "juce_core/juce_core.h"
ModMap::ModMap() {
  for (auto& dest : depthArr) {
    dest.fill(0.0f);
  }
  for (auto& dest : boolArr) {
    dest.fill(false);
  }
}

void ModMap::clearBoolGrid() {
  for (auto& dest : boolArr) {
    dest.fill(false);
  }
}

void ModMap::updateMap(ValueTree modTree) {
  jassert(modTree.hasType(ID::ELECTRUM_MOD_TREE));
  clearBoolGrid();
  for (auto it = modTree.begin(); it != modTree.end(); ++it) {
    auto child = *it;
    jassert(child.hasType(ID::ELECTRUM_MODULATION));
    const int src = child[ID::modSourceID];
    const int dest = child[ID::modDestID];
    const float depth = child[ID::modDepth];
    boolArr[(size_t)src][(size_t)dest] = true;
    depthArr[(size_t)src][(size_t)dest] = depth;
  }
}

bool ModMap::modExists(int src, int dest) const {
  return boolArr[(size_t)src][(size_t)dest];
}

int ModMap::numSourcesOnDest(int _dest) const {
  size_t dest = (size_t)_dest;
  int count = 0;
  for (size_t src = 0; src < MOD_SOURCES; ++src) {
    if (boolArr[src][dest]) {
      ++count;
    }
  }
  return count;
}

std::vector<mod_src_t> ModMap::getSourcesFor(int _dest) {
  size_t dest = (size_t)_dest;
  std::vector<mod_src_t> vec = {};
  for (size_t src = 0; src < MOD_SOURCES; ++src) {
    if (boolArr[src][dest]) {
      vec.push_back({(int)src, depthArr[src][dest]});
    }
  }
  return vec;
}

/*This loads the only the modulations in use into the first part of the
 * 'moc_src_t' array at 'arr' and sets the value of 'numSources' to
 * the number of sources in use*/
void ModMap::getSourcesSafe(mod_src_t* arr, int* numSources, int destID) const {
  *numSources = 0;
  for (size_t src = 0; src < MOD_SOURCES; ++src) {
    if (boolArr[src][(size_t)destID]) {
      auto* srcPtr = &arr[*numSources];
      srcPtr->source = (int)src;
      srcPtr->depth = depthArr[src][(size_t)destID];
      *numSources += 1;
    }
  }
}

//===================================================

static std::array<String, MOD_DESTS> _getModDestParamIDs() {
  std::array<String, MOD_DESTS> arr;
  size_t modDest = 0;
  int oscIdx = 0;
  int filterIdx = 0;
  while (modDest < MOD_DESTS) {
    if (modDest <= ModDestE::osc3Pos) {
      String iStr(oscIdx);
      String coarseID = ID::oscillatorCoarseTune.toString() + iStr;
      arr[modDest] = coarseID;
      ++modDest;
      String fineID = ID::oscillatorFineTune.toString() + iStr;
      arr[modDest] = fineID;
      ++modDest;
      String posID = ID::oscillatorPos.toString() + iStr;
      arr[modDest] = posID;
      ++modDest;
      String levelID = ID::oscillatorLevel.toString() + iStr;
      arr[modDest] = levelID;
      ++modDest;
      String panID = ID::oscillatorPan.toString() + iStr;
      arr[modDest] = panID;
      ++modDest;
      ++oscIdx;
    } else if (modDest <= ModDestE::filt2Gain) {
      String iStr(filterIdx);
      String cutoffID = ID::filterCutoff.toString() + iStr;
      arr[modDest] = cutoffID;
      ++modDest;
      String resID = ID::filterResonance.toString() + iStr;
      arr[modDest] = resID;
      ++modDest;
      String gainID = ID::filterGainDb.toString() + iStr;
      arr[modDest] = gainID;
      ++modDest;
      ++filterIdx;
    }
  }
  return arr;
}

String paramIDForModDest(int destID) {
  static std::array<String, MOD_DESTS> destIDs = _getModDestParamIDs();
  return destIDs[(size_t)destID];
}

static String s_defaultSineLFOString() {
  std::vector<lfo_handle_t> handles = {};
  const size_t numHandles = 55;
  for (size_t i = 0; i < numHandles; ++i) {
    float normPhase = (float)i / (float)numHandles;
    int bin = (int)(normPhase * (float)(LFO_SIZE - 1));
    float lvl = (float)std::sin(normPhase * juce::MathConstants<float>::twoPi);
    lvl = (lvl + 1.0f) * 0.5f;
    handles.push_back({bin, lvl});
  }
  // float lastNormPhase = (float)(LFO_SIZE - 1) / (float)(LFO_SIZE);
  // float lvl = (float)std::sin(
  //     (float(std::sin(lastNormPhase * juce::MathConstants<float>::twoPi))));
  // handles.push_back({LFO_SIZE - 1, lvl});
  return LFO::stringEncode(handles);
}

static ValueTree s_makeLfoInfoTree() {
  ValueTree vt(ID::LFO_INFO);
  const String defaultLfo = s_defaultSineLFOString();
  for (int i = 0; i < NUM_LFOS; ++i) {
    String propName = ID::lfoShapeString.toString() + String(i);
    vt.setProperty(propName, defaultLfo, nullptr);
  }
  return vt;
}

ElectrumState::ElectrumState(juce::AudioProcessor& proc,
                             juce::UndoManager* undo)
    : apvts(proc, undo, ID::ELECTRUM_STATE, ID::getParameterLayout()) {
  // 1. add the default modulation tree
  ValueTree mod(ID::ELECTRUM_MOD_TREE);
  state.appendChild(mod, undo);
  // now we initialize the modDestRanges array
  // remember this is in order of the ModDestE enum
  for (int i = 0; i < MOD_DESTS; ++i) {
    modDestRanges[i] = getParameterRange(paramIDForModDest(i));
  }
  // 2. make sure our waveIndex array doesn't have garbage
  for (size_t i = 0; i < NUM_OSCILLATORS; ++i) {
    lastWaveIndices[i] = 0;
  }
  // 3. add our LFO info child tree
  ensureLFOTree();
}

void ElectrumState::ensureLFOTree() {
  auto existing = state.getChildWithName(ID::LFO_INFO);
  if (!existing.isValid()) {
    // state.removeChild(existing, undoManager);
    auto lfoTree = s_makeLfoInfoTree();
    state.appendChild(lfoTree, undoManager);
  }
}

float ElectrumState::getModulatedDestValue(int destID,
                                           float baseValue,
                                           float modNorm) const {
  float bNorm = modDestRanges[destID].convertTo0to1(baseValue);
  bNorm = std::clamp(bNorm + modNorm, 0.0f, 1.0f);
  return modDestRanges[destID].convertFrom0to1(bNorm);
}

ValueTree ElectrumState::findTreeForRouting(const ValueTree& modTree,
                                            int src,
                                            int dest) {
  for (auto it = modTree.begin(); it != modTree.end(); ++it) {
    auto mod = *it;
    jassert(mod.hasType(ID::ELECTRUM_MODULATION));
    int _src = mod[ID::modSourceID];
    int _dest = mod[ID::modDestID];
    if (src == _src && dest == _dest)
      return mod;
  }
  return ValueTree();
}

void ElectrumState::setModulation(int src, int dest, float depth) {
  auto modTree = getModulationTree();
  auto child = findTreeForRouting(modTree, src, dest);
  if (child.isValid()) {  // this modulation already exists, we just update its
                          // depth
    child.setProperty(ID::modDepth, depth, undoManager);
  } else {  // create a new child for the modulation
    ValueTree newMod(ID::ELECTRUM_MODULATION);
    newMod.setProperty(ID::modSourceID, src, undoManager);
    newMod.setProperty(ID::modDestID, dest, undoManager);
    newMod.setProperty(ID::modDepth, depth, undoManager);
    modTree.appendChild(newMod, undoManager);
  }
}

void ElectrumState::removeModulation(int src, int dest) {
  auto modTree = getModulationTree();
  auto child = findTreeForRouting(modTree, src, dest);
  jassert(child.isValid());
  modTree.removeChild(child, undoManager);
}

bool ElectrumState::modulationExists(int src, int dest) {
  auto modTree = getModulationTree();
  for (auto it = modTree.begin(); it != modTree.end(); ++it) {
    auto mod = *it;
    const int _src = mod[ID::modSourceID];
    const int _dest = mod[ID::modDestID];
    if (_src == src && _dest == dest)
      return true;
  }
  return false;
}

float ElectrumState::modulationDepth(int src, int dest) {
  auto modTree = getModulationTree();
  auto mod = findTreeForRouting(modTree, src, dest);
  jassert(mod.isValid());
  return (float)mod[ID::modDepth];
}

//============================================================

void ElectrumState::updateCommonAudioData() {
  // oscillators----------------------------------
  for (int i = 0; i < NUM_OSCILLATORS; ++i) {
    String iStr(i);
    // 1. figure out the IDs
    const String posID = ID::oscillatorPos.toString() + iStr;
    const String waveID = ID::oscillatorWaveIndex.toString() + iStr;
    const String levelID = ID::oscillatorLevel.toString() + iStr;
    const String coarseID = ID::oscillatorCoarseTune.toString() + iStr;
    const String fineID = ID::oscillatorFineTune.toString() + iStr;
    const String panID = ID::oscillatorPan.toString() + iStr;
    const String activeID = ID::oscillatorActive.toString() + iStr;
    // 2. grab from the atomic values
    const bool _active = getRawParameterValue(activeID)->load() > 0.5f;
    const float _pos = getRawParameterValue(posID)->load();
    const float _level = getRawParameterValue(levelID)->load();
    const float _coarse = getRawParameterValue(coarseID)->load();
    const float _fine = getRawParameterValue(fineID)->load();
    const float _pan = getRawParameterValue(panID)->load();
    const int _waveIdx = (int)getRawParameterValue(waveID)->load();
    // 3. replace the wavetable data if needed
    if (_waveIdx != lastWaveIndices[(size_t)i]) {
      lastWaveIndices[(size_t)i] = _waveIdx;
      auto* data = userLib.getWavetableData((int)_waveIdx);
      jassert(data != nullptr);
      auto waveStr = UserFiles::loadTableStringForWave(data->name);
      audioData.wOsc[i].loadWaveData(waveStr);
    }
    // 4. assign to the DSP objects
    audioData.wOsc[i].setPos(_pos);
    audioData.wOsc[i].setActive(_active);
    audioData.wOsc[i].setLevel(_level);
    audioData.wOsc[i].setCoarse(_coarse);
    audioData.wOsc[i].setFine(_fine);
    audioData.wOsc[i].setPan(_pan);
  }
  // envelopes----------------------------------
  for (int i = 0; i < NUM_ENVELOPES; ++i) {
    String iStr(i);
    const String aMsID = ID::attackMs.toString() + iStr;
    const String aCurveID = ID::attackCurve.toString() + iStr;
    const String hID = ID::holdMs.toString() + iStr;
    const String dMsID = ID::decayMs.toString() + iStr;
    const String dCurveID = ID::decayCurve.toString() + iStr;
    const String sID = ID::sustainLevel.toString() + iStr;
    const String rMsID = ID::releaseMs.toString() + iStr;
    const String rCurveID = ID::releaseCurve.toString() + iStr;
    const String vID = ID::velocityTracking.toString() + iStr;

    ahdsr_data_t envParams;
    envParams.attackMs = getRawParameterValue(aMsID)->load();
    envParams.attackCurve = getRawParameterValue(aCurveID)->load();
    envParams.holdMs = getRawParameterValue(hID)->load();
    envParams.decayMs = getRawParameterValue(dMsID)->load();
    envParams.decayCurve = getRawParameterValue(dCurveID)->load();
    envParams.sustainLevel = getRawParameterValue(sID)->load();
    envParams.velTracking = getRawParameterValue(vID)->load();
    envParams.releaseMs = getRawParameterValue(rMsID)->load();
    envParams.releaseCurve = getRawParameterValue(rCurveID)->load();

    audioData.env[i].updateState(envParams);
  }
  // filters-------------------------------------------
  // this frange helps us convert the atomically
  // read float value into an integer type ID
  static frange_t fTypeRange =
      getParameterRange(ID::filterType.toString() + "0");
  for (int i = 0; i < NUM_FILTERS; ++i) {
    String iStr(i);
    const String activeID = ID::filterActive.toString() + iStr;
    const String cutoffID = ID::filterCutoff.toString() + iStr;
    const String resID = ID::filterResonance.toString() + iStr;
    const String gainID = ID::filterGainDb.toString() + iStr;
    const String typeID = ID::filterType.toString() + iStr;

    bool _isActive = getRawParameterValue(activeID)->load() > 0.5f;
    const float _cutoff = getRawParameterValue(cutoffID)->load();
    const float _res = getRawParameterValue(resID)->load();
    const float _gainDb = getRawParameterValue(gainID)->load();
    const float _fType = getRawParameterValue(typeID)->load();
    const float normType = fTypeRange.convertTo0to1(_fType);
    audioData.filters[i].filterType =
        (FilterTypeE)(normType * (float)(NUM_FILTER_TYPES - 1));
    audioData.filters[i].active = _isActive;
    audioData.filters[i].baseCutoff = _cutoff;
    audioData.filters[i].baseResLin = _res;
    audioData.filters[i].baseGainLin = juce::Decibels::decibelsToGain(_gainDb);
  }
  // LFOs----------------------------------------------------
  for (int i = 0; i < NUM_LFOS; ++i) {
    audioData.lfos[i].updateData(*this, i);
    String iStr(i);
    const String freqID = ID::lfoFrequencyHz.toString() + iStr;
    const String trigModeId = ID::lfoTriggerMode.toString() + iStr;

    const float _freq = getRawParameterValue(freqID)->load();
    const float _fMode = getRawParameterValue(trigModeId)->load();
    audioData.lfos[i].setHz(_freq);
    audioData.lfos[i].setTriggerMode(_fMode);
  }
}

void ElectrumState::updateLFOString(const String& shapeString, int lfoID) {
  juce::ignoreUnused(shapeString, lfoID);
  // auto lfoTree = state.getChildWithName(ID::LFO_INFO);
  // if (lfoTree.isValid()) {
  //   String shapeStringID = ID::lfoShapeString.toString() + String(lfoID);
  //   lfoTree.setProperty(shapeStringID, shapeString, nullptr);
  // } else {
  //   jassert(false);
  // }
}

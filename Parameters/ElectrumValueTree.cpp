#include "ElectrumValueTree.h"
#include "ElectrumVoicesState.h"
#include "Identifiers.h"
ModDestMap::ModDestMap()
{
  size_t idx = 0;
  modArr[idx].destID = IDs::filterCutoff.toString();
  ++idx;
  modArr[idx].destID = IDs::filterResonance.toString();
  ++idx;
  modArr[idx].destID = IDs::filterMix.toString();
  ++idx;
  modArr[idx].destID = IDs::filterTracking.toString();
  ++idx;
  for (int o = 0; o < NUM_OSCILLATORS; o++)
  {
    modArr[idx].destID = IDs::oscillatorPos.toString() + String(o);
    ++idx;
    modArr[idx].destID = IDs::oscillatorLevel.toString() + String(o);
    ++idx;
    modArr[idx].destID = IDs::oscillatorPan.toString() + String(o);
    ++idx;
    modArr[idx].destID = IDs::oscillatorCoarseTune.toString() + String(o);
    ++idx;
    modArr[idx].destID = IDs::oscillatorFineTune.toString() + String(o);
    ++idx;
  }
  modArr[idx].destID = IDs::saturationCoeff.toString();
  ++idx;
  modArr[idx].destID = IDs::saturationDrive.toString();
  ++idx;
  modArr[idx].destID = IDs::saturationMix.toString();
  ++idx;
  DLog::log(String(idx) + " mod destinations initialized");
}

void ModDestMap::loadFromTree(ValueTree &modTree)
{
  for (auto &mod : modArr)
  {
    mod.clearMods();
  }
  for (auto it = modTree.begin(); it != modTree.end(); ++it)
  {
    auto tree = *it;
    if (tree.hasType(IDs::MODULATION))
    {
      String srcID = tree[IDs::modulationSource];
      String destID = tree[IDs::modulationDest];
      float depth = tree[IDs::modulationDepth];
      // if this is already in the map, add it to the end of the list
      for (auto &mod : modArr)
      {
        if (mod.destID == destID)
        {
          mod.setMod(srcID, depth);
          break;
        }
      }
    }
  }
}

ModulationDestList::ModulationDestList(const String &id) : destID(id) {}

ModulationDestList::ModulationDestList(const String &id, ValueTree &tree) : destID(id)
{
  for (auto it = tree.begin(); it != tree.end(); ++it)
  {
    auto modTree = *it;
    if (tree.hasType(IDs::MODULATION))
    {
      String srcID = modTree[IDs::modulationSource];
      float depth = modTree[IDs::modulationDepth];
      mods.add(new Modulation(srcID, depth));
    }
  }
}

void ModulationDestList::addMod(const String &srcID, float depth)
{
  mods.add(new Modulation(srcID, depth));
}

void ModulationDestList::removeMod(const String &srcID)
{
  for (auto *mod : mods)
  {
    if (mod->sourceID == srcID)
    {
      mods.removeObject(mod, true);
      return;
    }
  }
}

void ModulationDestList::setMod(const String &srcID, float depth)
{
  if (!hasMod(srcID))
  {
    addMod(srcID, depth);
    return;
  }
  for (auto *mod : mods)
  {
    if (mod->sourceID == srcID)
    {
      mod->depth = depth;
      return;
    }
  }
}

bool ModulationDestList::hasMod(const String &srcID)
{
  for (auto *mod : mods)
  {
    if (mod->sourceID == srcID)
    {
      return true;
    }
  }
  return false;
}

OwnedArray<Modulation> *ModDestMap::getModsFor(const String &destID)
{
  for (auto &dest : modArr)
  {
    if (dest.destID == destID)
      return &dest.getMods();
  }
  return nullptr;
}
//===============================================================================
ValueTree EVT::getModulationsTree()
{
  auto modTree = coreTree.state.getChildWithName(IDs::ELECTRUM_MODULATIONS);
  if (modTree.isValid())
    return modTree;
  else
  {
    coreTree.state.appendChild(createModulationsTree(), nullptr);
    return coreTree.state.getChildWithName(IDs::ELECTRUM_MODULATIONS);
  }
}

std::vector<ValueTree> EVT::getModulations()
{
  std::vector<ValueTree> mods;
  for (auto it = getModulationsTree().begin(); it != getModulationsTree().end(); ++it)
  {
    auto tree = *it;
    if (tree.hasType(IDs::MODULATION))
    {
      mods.push_back(tree);
    }
  }
  return mods;
}

ValueTree EVT::getModulation(const String &source, const String &dest)
{
  for (auto &mod : getModulations())
  {
    String src = mod[IDs::modulationSource];
    String dst = mod[IDs::modulationDest];
    if (source == src && dest == dst)
      return mod;
  }
  return ValueTree();
}

bool EVT::sourceIsInUse(const String &source)
{
  auto modTree = getModulationsTree();
  for (auto it = modTree.begin(); it != modTree.end(); ++it)
  {
    auto tree = *it;
    String srcID = tree[IDs::modulationSource];
    if (srcID == source)
      return true;
  }
  return false;
}

// use this to add or update the value of a modulation
void EVT::setModulation(const String &source, const String &dest, float depth)
{
  auto existing = getModulation(source, dest);
  if (existing.isValid())
  {
    existing.setProperty(IDs::modulationDepth, depth, nullptr);
  } else
  {
    auto newMod = createModulationTree(source, dest, depth);
    getModulationsTree().appendChild(newMod, nullptr);
  }
}
// check if a given modulation exists
bool EVT::modulationExists(const String &source, const String &dest)
{
  auto mod = getModulation(source, dest);
  return mod.isValid();
}
// removes a modulation routing
void EVT::removeModulation(const String &src, const String &dest)
{
  auto mod = getModulation(src, dest);
  // make sure we're removing a modulation that actually exists
  jassert(mod.isValid());
  getModulationsTree().removeChild(mod, nullptr);
}
void EVT::loadModulationData(ModDestMap &modMap)
{
  TRACE_DSP();
  auto tree = getModulationsTree();
  modMap.loadFromTree(tree);
}
//===============================================================
void EVT::updateEnvelopesForBlock()
{
  TRACE_DSP();
  for (int i = 0; i < NUM_ENVELOPES; i++)
  {
    // step 1: grip atomic values from the tree;
    String iStr(i);
    String srcID = IDs::envSource.toString() + iStr;
    envsInUse = ElectrumVoicesState::setVoice(envsInUse, i, sourceIsInUse(srcID));
    const float atkMs = getFloatParamValue(IDs::attackMs.toString() + iStr);
    const float atkCrv = getFloatParamValue(IDs::attackCurve.toString() + iStr);

    const float holdMs = getFloatParamValue(IDs::holdMs.toString() + iStr);

    const float dcyMs = getFloatParamValue(IDs::decayMs.toString() + iStr);

    const float decayCrv = getFloatParamValue(IDs::decayCurve.toString() + iStr);

    const float sustainLvl = getFloatParamValue(IDs::sustainLevel.toString() + iStr);

    const float velTracking = getFloatParamValue(IDs::velocityTracking.toString() + iStr);

    const float relMs = getFloatParamValue(IDs::releaseMs.toString() + iStr);
    const float releaseCrv = getFloatParamValue(IDs::releaseCurve.toString() + iStr);

    // step 2- save the values in the ElectrumAudioData instance used by the
    // audio thread
    auto envData = audioData->getEnvelopeData(i);
    envData->attackMs = atkMs;
    envData->attackCurve = atkCrv;

    envData->holdMs = holdMs;

    envData->decayMs = dcyMs;
    envData->decayCurve = decayCrv;

    envData->sustainLevel = sustainLvl;

    envData->velTracking = velTracking;

    envData->releaseMs = relMs;
    envData->releaseCurve = releaseCrv;
  }
}

void EVT::updateLFOsForBlock()
{
  for (int i = 0; i < NUM_LFOS; i++)
  {
    String iStr(i);
    const float freq = getFloatParamValue(IDs::lfoFreq.toString() + iStr);
    const float curveA = getFloatParamValue(IDs::lfoMidpointA.toString() + iStr);
    const float center = getFloatParamValue(IDs::lfoCenterX.toString() + iStr);
    const float curveB = getFloatParamValue(IDs::lfoMidpointB.toString() + iStr);

    auto data = audioData->getLFOData(i);
    data->frequency = freq;
    data->curveA = curveA;
    data->center = center;
    data->curveB = curveB;
  }
}

void EVT::parameterChanged(const String &paramID, float value)
{
  if (paramID == IDs::filterType.toString())
  {
    DLog::log("New filter type has value: " + String(value));
    auto fMax = (float)(IDs::filterTypes.size() - 1);
    float fIndex = jmap(value, 0.0f, 1.0f, 0.0f, fMax);
    int index = (int)fIndex;
    DLog::log("Selected index: " + String(index));
    currentFilterType = IDs::filterTypes[index];
  }
}

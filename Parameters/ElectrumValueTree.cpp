#include "ElectrumValueTree.h"
#include "ElectrumVoicesState.h"
#include "Identifiers.h"

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



bool EVT::sourceIsInUse(const String& source)
{
  auto modTree = getModulationsTree();
  for(auto it = modTree.begin(); it != modTree.end(); ++it)
  {
    auto tree = *it;
    String srcID = tree[IDs::modulationSource];
    if(srcID == source)
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
    }
    else
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
void EVT::loadModulationData(ModDestMap& modMap)
{
    TRACE_DSP();
    modMap.clear();
    auto currentMods = getModulations();
    for(auto& mod : currentMods)
    {
        // grip some data
        String src = mod[IDs::modulationSource];
        String dest = mod[IDs::modulationDest];
        float depth = mod[IDs::modulationDepth];
        // first, check if the dest exists as a key yet
        auto destIt = modMap.find(dest);
        // if it does, add the source and depth to the appropriate hashmap
        if (destIt != modMap.end())
        {
            modMap[dest][src] = depth;
        }
        // otherwise set it to a new hashmap with just this source and dest
        else
        {
            modMap[dest] = {{src, depth}};
        }
    }

}
//===============================================================
void EVT::updateEnvelopesForBlock()
{
    TRACE_DSP();
    for(int i = 0; i < NUM_ENVELOPES; i++)
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

        const float relMs = getFloatParamValue(IDs::releaseMs.toString() + iStr);
        const float releaseCrv = getFloatParamValue(IDs::releaseCurve.toString() + iStr);

        // step 2- save the values in the ElectrumAudioData instance used by the audio thread
        auto envData = audioData->getEnvelopeData(i);
        envData->attackMs = atkMs;
        envData->attackCurve = atkCrv;

        envData->holdMs = holdMs;

        envData->decayMs = dcyMs;
        envData->decayCurve = decayCrv;

        envData->sustainLevel = sustainLvl;

        envData->releaseMs = relMs;
        envData->releaseCurve = releaseCrv;
    }
}


void EVT::parameterChanged(const String& paramID, float value) 
{
  if(paramID == IDs::filterType.toString())
  {
    DLog::log("New filter type has value: " + String(value));
    auto fMax = (float)(IDs::filterTypes.size() - 1);
    float fIndex = jmap(value, 0.0f, 1.0f, 0.0f, fMax);
    int index = (int)fIndex;
    DLog::log("Selected index: " + String(index));
    currentFilterType = IDs::filterTypes[index];
  }
}

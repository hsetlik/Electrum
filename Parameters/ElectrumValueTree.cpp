#include "ElectrumValueTree.h"

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
        const float atkMs = getFloatParamValue(IDs::attackMs.toString() + iStr);
        const float aCL1 = getFloatParamValue(IDs::attackC1Length.toString() + iStr);
        const float aCA1 = getFloatParamValue(IDs::attackC1Angle.toString() + iStr);
        const float aCL2 = getFloatParamValue(IDs::attackC2Length.toString() + iStr);
        const float aCA2 = getFloatParamValue(IDs::attackC2Angle.toString() + iStr);

        const float holdMs = getFloatParamValue(IDs::holdMs.toString() + iStr);
        
        const float dcyMs = getFloatParamValue(IDs::decayMs.toString() + iStr);
        const float decCL1 = getFloatParamValue(IDs::decayC1Length.toString() + iStr);
        const float decCA1 = getFloatParamValue(IDs::decayC1Angle.toString() + iStr);       
        const float decCL2 = getFloatParamValue(IDs::decayC2Length.toString() + iStr);
        const float decCA2 = getFloatParamValue(IDs::decayC2Angle.toString() + iStr); 

        const float sustainLvl = getFloatParamValue(IDs::sustainLevel.toString() + iStr);

        const float relMs = getFloatParamValue(IDs::releaseMs.toString() + iStr);
        const float relCL1 = getFloatParamValue(IDs::releaseC1Length.toString() + iStr);
        const float relCA1 = getFloatParamValue(IDs::releaseC1Angle.toString() + iStr);       
        const float relCL2 = getFloatParamValue(IDs::releaseC2Length.toString() + iStr);
        const float relCA2 = getFloatParamValue(IDs::releaseC2Angle.toString() + iStr); 
        // step 2- save the values in the ElectrumAudioData instance used by the audio thread
        auto envData = audioData->getEnvelopeData(i);
        envData->attackMs = atkMs;
        envData->attackC1Angle = aCA1;
        envData->attackC1Length = aCL1;
        envData->attackC2Angle = aCA2;
        envData->attackC2Length = aCL2;

        envData->holdMs = holdMs;

        envData->decayMs = dcyMs;
        envData->decayC1Angle = decCA1;
        envData->decayC1Length = decCL1;
        envData->decayC2Angle = decCA2;
        envData->decayC2Length = decCL2; 
    
        envData->sustainLevel = sustainLvl;

        envData->releaseMs = relMs;
        envData->releaseC1Angle = relCA1;
        envData->releaseC1Length = relCL1;
        envData->releaseC2Angle = relCA2;
        envData->releaseC2Length = relCL2; 
    }
}
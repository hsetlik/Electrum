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
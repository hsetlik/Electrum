#pragma once
#include "Identifiers.h"
#include "ElectrumAudioData.h"

class EVT
{
private:
    static ValueTree createModulationsTree()
    {
        ValueTree tree(IDs::ELECTRUM_MODULATIONS);
        return tree;
    }
    static ValueTree createModulationTree(const String& source, const String& dest, float depth)
    {
        ValueTree tree(IDs::MODULATION);
        tree.setProperty(IDs::modulationSource, source, nullptr);
        tree.setProperty(IDs::modulationDest, dest, nullptr);
        tree.setProperty(IDs::modulationDepth, depth, nullptr);
        return tree;
    }
    ValueTree getModulationsTree()
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
    std::vector<ValueTree> getModulations()
    {
        std::vector<ValueTree> mods;
        for(auto it = getModulationsTree().begin(); it != getModulationsTree().end(); ++it)
        {
            auto tree = *it;
            if (tree.hasType(IDs::MODULATION))
            {
                mods.push_back(tree);
            }
        }
        return mods;
    }
    ValueTree getModulation(const String& source, const String& dest)
    {
        for(auto& mod : getModulations())
        {
            String src = mod[IDs::modulationSource];
            String dst = mod[IDs::modulationDest];
            if (source == src && dest == dst)
                return mod;
        }
        return ValueTree();
    }
    //state
    std::unique_ptr<ElectrumAudioData> audioData;
    APVTS coreTree;
public:
    EVT(AudioProcessor &proc,
         UndoManager *undo,
         const Identifier &valueTreeType) : 
         audioData(std::make_unique<ElectrumAudioData>()),
         coreTree(proc, undo, valueTreeType, IDs::createElectrumLayout())
    {


    }

    float getOscillatorValue(int idx, float phase, float tablePos, double freq, double sampleRate)
    {
        if (audioData != nullptr)
            return audioData->getOscillatorValue(idx, phase, tablePos, freq, sampleRate);
        return 0.0f;
    }
    APVTS* getAPVTS() { return &coreTree; }
    // Modulation info stuff
    //use this to add or update the value of a modulation
    void setModulation(const String& source, const String& dest, float depth)
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
    bool modulationExists(const String& source, const String& dest)
    {
        auto mod = getModulation(source, dest);
        return mod.isValid();
    }
    // get an unordered_map of all the sources and depths for a given source
    std::unordered_map<String, float> getModulationsForDest(const String& dest)
    {
        std::unordered_map<String, float> out;
        for(auto mod : getModulations())
        {
            String dst = mod[IDs::modulationDest];
            if (dst == dest)
            {
                String src = mod[IDs::modulationSource];
                float depth = mod[IDs::modulationDepth];
                out[src] = depth;
            }
        }
        return out;
    }

};
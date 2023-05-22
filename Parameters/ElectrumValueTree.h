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

    std::unique_ptr<ElectrumAudioData> audioData;
    APVTS coreTree;
public:
    EVT(AudioProcessor &proc,
         UndoManager *undo,
         const Identifier &valueTreeType) : 
         coreTree(proc, undo, valueTreeType, IDs::createElectrumLayout()),
         audioData(std::make_unique<ElectrumAudioData>())
    {


    }

    float getOscillatorValue(int idx, float phase, float tablePos, double freq, double sampleRate)
    {
        if (audioData != nullptr)
            return audioData->getOscillatorValue(idx, phase, tablePos, freq, sampleRate);
        return 0.0f;
    }

    APVTS* getAPVTS() { return &coreTree; }

};
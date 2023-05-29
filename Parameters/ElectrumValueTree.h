#pragma once
#include "Identifiers.h"
#include "ElectrumAudioData.h"
#include "AudioSystem.h"
#include "../GUI/Color.h"
#include "../Audio/Modulators/Perlin.h"

// this works like
using ModDestMap = std::unordered_map<String, std::unordered_map<String, float>>;

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
//=================================================================================
    ValueTree getModulationsTree();
    std::vector<ValueTree> getModulations();
    ValueTree getModulation(const String& source, const String& dest);
//========================================================================================
    //state
    std::unique_ptr<ElectrumAudioData> audioData;
    APVTS coreTree;
    std::atomic<bool> sustainPedalOn;
    std::atomic<float> modWheelValue;
    std::atomic<float> pitchBendValue;

//========================================================================================
    // perlin stuff
    PerlinGenerator perlin;
    std::atomic<float> lastPerlinVal;
public:
    EVT(AudioProcessor &proc,
         UndoManager *undo,
         const Identifier &valueTreeType) : 
         audioData(std::make_unique<ElectrumAudioData>()),
         coreTree(proc, undo, valueTreeType, IDs::createElectrumLayout()),
         sustainPedalOn(false),
         modWheelValue(0.0f),
         pitchBendValue(0.0f),
         lastPerlinVal(0.0f)
    {


    }

    ElectrumAudioData* getAudioData() { return audioData.get(); }
    //helper functions for accesing the underlying atomic values. unchecked!
    float getFloatParamValue(const String& id)
    {
        if(auto param = dynamic_cast<AudioParameterFloat*>(coreTree.getParameter(id)))
        {
            return param->get();
        }
        else
        {
            DLog::log("Could not get float parameter with ID: " + id);
            return 0.0f;
        }
    }
    int getIntParamValue(const String& id)
    {
        if(auto param = dynamic_cast<AudioParameterInt*>(coreTree.getParameter(id)))
        {
            return param->get();
        }
        else
        {
            DLog::log("Could not get int parameter with ID: " + id);
            return -1;
        }
    }
    // Since the perlin noise generator is shared by all voices, we handle it here
    // updates the perlin noise generator with any UI changes
    void updatePerlinForBlock()
    {
        float freq = getFloatParamValue(IDs::perlinFreq.toString());
        float lac = getFloatParamValue(IDs::perlinLacunarity.toString());
        size_t octaves = (size_t)getIntParamValue(IDs::perlinOctaves.toString());
        perlin.setParams(octaves, freq, lac);
    }
    //advances the perlin generator for the next sample
    void tickPerlinForSample()
    {
        lastPerlinVal = perlin.getNextValue();
    }
    //gets the current output of the perlin generator
    float perlinValue() const
    {
        return lastPerlinVal.load();
    }

    float getOscillatorValue(int idx, float phase, float tablePos, double freq, double sampleRate)
    {
        return audioData->getOscillatorValue(idx, phase, tablePos, freq, sampleRate);
    }
    APVTS* getAPVTS() { return &coreTree; }
    // Modulation info stuff
    //use this to add or update the value of a modulation
    void setModulation(const String& source, const String& dest, float depth);

    // check if a given modulation exists
    bool modulationExists(const String& source, const String& dest);

    //removes a modulation routing
    void removeModulation(const String& src, const String& dest);
    //updates the AudioData with the current envelope parameters
    void updateEnvelopesForBlock();


    void setSustainPedal(bool shouldBeOn) { sustainPedalOn = shouldBeOn; }
    bool getSustainPedal() { return sustainPedalOn.load(); }

    void setModWheel(float val) { modWheelValue = val; }
    float getModWheel() { return modWheelValue.load(); }

    void setPitchBend(float val) { pitchBendValue = val; }
    float getPitchBend() { return pitchBendValue.load(); }

    // load the modulations to a structure we can access in a thread-safe way
    void loadModulationData(ModDestMap& modMap);

};